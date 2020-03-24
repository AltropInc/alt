#include "RingBuffer.h"
#include <util/sysinfo/SysConfig.h>

namespace alt
{

RingBuffer::BufferHeader::BufferHeader(size_t capacity)
    : owns_buffer_(true)
    , capacity_(power2Next(capacity))
    , mod_mask_(capacity_-1)
    , buffer_ (reinterpret_cast<char*>(::malloc(capacity_)))
{}

RingBuffer::BufferHeader::BufferHeader(char* buffer, size_t capacity)
    : owns_buffer_(false)
    , buffer_ (buffer)
    , capacity_(capacity)     // capacity must be power of 2
    , mod_mask_(capacity_-1)
{}

RingBuffer::RingBuffer(size_t capacity)
    : header_(capacity)
{}

RingBuffer::RingBuffer(char* buffer, size_t capacity)
    : header_(buffer, capacity)
{}

RingBuffer::RingBuffer()
{}

void RingBuffer::setBuffer(char* buffer, size_t capacity)
{
    header_.buffer_ = buffer;
    header_.capacity_ = capacity;       // capacity must be power of 2
    header_.mod_mask_ = (capacity-1);
}

RingBuffer::~RingBuffer()
{
    if (header_.owns_buffer_)
    {
        ::free(header_.buffer_);
    }
}

size_t RingBuffer::requiredHeaderSize()
{
    return constAlign(sizeof(BufferHeader), SysConfig::instance().cache_line_size_);
}

size_t RingBuffer::requiredSize(size_t capacity)
{
    return requiredHeaderSize() + power2Next(capacity);
}

RingBuffer* RingBuffer::create(char* addr,  const MemoryAttrs& attrs, size_t capacity)
{
    RingBuffer* rbuff(nullptr);
    if (attrs.is_new_)
    {
        char* buffer = addr + requiredHeaderSize();
        rbuff = new (addr) RingBuffer(buffer, capacity);
    }
    else
    {
        rbuff = reinterpret_cast<RingBuffer*>(addr);
    }
    return rbuff;
}

bool RingBuffer::hasFreeSpace(size_t required_non_split_space, size_t required_space) const
{
    size_t read_pos = header_.read_pos_.load(std::memory_order_acquire);
    size_t write_pos = header_.write_pos_.load(std::memory_order_relaxed);
    size_t free_space = header_.capacity_ - (write_pos - read_pos);
    size_t waste_sapce = 0;
    if (required_non_split_space > 0)
    {
        size_t rp = read_pos & header_.mod_mask_;
        size_t wp = write_pos & header_.mod_mask_;
        if (wp > rp && header_.capacity_ - wp < required_non_split_space)
        {
            waste_sapce = header_.capacity_ - wp;
        }
    }

    return free_space >= required_space + required_non_split_space + waste_sapce;
}

bool RingBuffer::write (const char* buff, size_t len, bool split)
{
    size_t read_pos = header_.read_pos_.load(std::memory_order_acquire);
    size_t write_pos = header_.write_pos_.load(std::memory_order_relaxed);
    if (write_pos-read_pos >= header_.capacity_)
    {
        // free space is not sufficent
        return false;
    }
    size_t rp = read_pos & header_.mod_mask_;
    size_t wp = write_pos & header_.mod_mask_;
    if (wp > rp)
    {
        // unwrapped case
        if (header_.capacity_ - wp > len)
        {
            // tail space is sufficent for the new data
            memcpy(header_.buffer_ + wp, buff, len);
            header_.write_pos_.store(write_pos + len, std::memory_order_release);
        }
        else if (split)
        {
            // wrap the data in two portions
            uint64_t end_sz = header_.capacity_ - wp;
            if (end_sz>0)
            {
                memcpy(header_.buffer_ + wp, buff, end_sz);
            }
            memcpy(header_.buffer_, buff + end_sz, len - end_sz);
            header_.wasted_space_.store(0, std::memory_order_release);
            header_.write_pos_.store(write_pos + len, std::memory_order_release);
        }
        else if (rp < len)
        {
            // space is not sufficent because of the wasted space
            return false;
        }
        else
        {
            memcpy(header_.buffer_, buff, len);
            size_t wasted_space = header_.capacity_ - wp;
            header_.wasted_space_.store(wasted_space, std::memory_order_release);
            header_.write_pos_.store(write_pos + len + wasted_space, std::memory_order_release);
        }
    }
    else
    {
        memcpy(header_.buffer_ + wp, buff, len);
        header_.write_pos_.store(write_pos + len, std::memory_order_release);
    }
    return true;
}

size_t RingBuffer::fetchAll(struct iovec* iov)
{
    size_t read_pos = header_.read_pos_.load(std::memory_order_relaxed);
    size_t write_pos = header_.write_pos_.load(std::memory_order_acquire);
    if (write_pos==read_pos)
    {
        iov[0].iov_len = 0;
        iov[1].iov_len = 0;
        return 0;
    }
    size_t rp = read_pos & header_.mod_mask_;
    size_t wp = write_pos & header_.mod_mask_;
    size_t data_size;
    if (wp > rp)
    {
        data_size =  wp - rp;
        iov[0].iov_base =header_.buffer_ + rp;
        iov[0].iov_len = data_size;
        iov[1].iov_len = 0;
    }
    else if (header_.capacity_ - rp > header_.wasted_space_)
    {
        // data is wrapped
        iov[1].iov_base = header_.buffer_;
        iov[1].iov_len = wp;
        iov[0].iov_base = header_.buffer_ + rp;
        iov[0].iov_len = header_.capacity_ - rp - header_.wasted_space_;
        data_size = iov[0].iov_len + iov[1].iov_len;
    }
    else
    {
        data_size =  wp;
        iov[0].iov_base = header_.buffer_;
        iov[0].iov_len = data_size;
        iov[1].iov_len = 0;
    }
    
    header_.commit_pos_ = write_pos;
    return data_size;
}

// Non-copy read
size_t RingBuffer::fetch(struct iovec* iov,  size_t len)
{
    size_t read_pos = header_.read_pos_.load(std::memory_order_relaxed);
    return fetch_i(iov, len, read_pos);
}

size_t RingBuffer::fetchNext(struct iovec* iov, size_t len)
{
    return fetch_i(iov, len, header_.commit_pos_);
}

size_t RingBuffer::fetch_i(struct iovec* iov,  size_t len, size_t read_pos)
{
    size_t write_pos = header_.write_pos_.load(std::memory_order_acquire);
    if (read_pos + len > write_pos)
    {
        iov[0].iov_len = 0;
        iov[1].iov_len = 0;
        return 0;
    }
    size_t rp = read_pos & header_.mod_mask_;
    size_t wp = write_pos & header_.mod_mask_;
    size_t data_size;
    if (wp > rp)
    {
        data_size =  wp - rp;
        if (data_size < len)
        {
            header_.commit_pos_ = header_.read_pos_.load(std::memory_order_relaxed);
            return 0;
        }
        iov[0].iov_base = iov[1].iov_base = header_.buffer_ + rp;
        iov[0].iov_len = len;
        iov[1].iov_len = 0;
        header_.commit_pos_ = read_pos + len;
        return len;
    }

    size_t wasted_space = header_.wasted_space_.load(std::memory_order_acquire);
    // buffer wrapped case
    if (header_.capacity_ - rp - wasted_space >= len)
    {
        // data is not wrapped
        iov[0].iov_base = header_.buffer_ + rp;
        iov[0].iov_len = len;
        iov[1].iov_len = 0;
        header_.commit_pos_ = read_pos + len;
        return len;
    }

    if (header_.capacity_ - rp > wasted_space)
    {
        // data wrapped and split
        uint64_t end_sz = header_.capacity_ - rp - wasted_space;
        if (end_sz + wp < len)
        {
            // data is incomplete
            header_.commit_pos_ = header_.read_pos_.load(std::memory_order_relaxed);;
            return 0;
        }
        iov[1].iov_base = header_.buffer_;
        iov[1].iov_len = len - end_sz;
        iov[0].iov_base = header_.buffer_ + rp;
        iov[0].iov_len = end_sz;
    }
    else
    {
        // data wrapped, but not split
        if (wp < len)
        {
            // data is incomplete
            header_.commit_pos_ = header_.read_pos_.load(std::memory_order_relaxed);;
            return 0;
        }
        iov[0].iov_base = iov[1].iov_base = header_.buffer_;
        iov[0].iov_len = len;
        iov[1].iov_len = 0;
    }
    header_.commit_pos_ = read_pos + len + wasted_space;
    return len;
}

void RingBuffer::commitRead()
{
    header_.read_pos_.store(header_.commit_pos_, std::memory_order_release);
}

size_t RingBuffer::read(char* buf, size_t len)
{
    size_t write_pos = header_.write_pos_.load(std::memory_order_acquire);
    size_t read_pos = header_.read_pos_.load(std::memory_order_relaxed);
    if (read_pos + len > write_pos)
    {
        return 0;
    }
    size_t rp = read_pos & header_.mod_mask_;
    size_t wp = write_pos & header_.mod_mask_;
    size_t data_size;
    if (wp > rp)
    {
        data_size =  wp - rp;
        if (data_size < len)
        {
            return 0;
        }
        memcpy(buf, header_.buffer_ + rp, len);
        header_.read_pos_.store(read_pos + len, std::memory_order_release);
        return len;
    }

    // buffer wrapped case
    size_t wasted_space = header_.wasted_space_.load(std::memory_order_acquire);
    if (header_.capacity_ - rp - wasted_space >= len)
    {
        // data is not wrapped
        memcpy(buf, header_.buffer_ + rp, len);
        header_.read_pos_.store(read_pos + len, std::memory_order_release);
        return len;
    }

    // data wrapped case
    if (header_.capacity_ - rp > wasted_space)
    {
        // data wrapped and split
        uint64_t end_sz = header_.capacity_ - rp - wasted_space;
        if (end_sz + wp < len)
        {
            // data is incomplete
            return 0;
        }
        memcpy(buf, header_.buffer_ + rp, end_sz);
        memcpy(buf+end_sz, header_.buffer_, len - end_sz);
        header_.read_pos_.store(read_pos + len, std::memory_order_release);
    }
    else
    {
        // data wrapped, but not split
        if (wp < len)
        {
            // data is incomplete
            return 0;
        }
        memcpy(buf, header_.buffer_, len);
        header_.read_pos_.store(read_pos + len, std::memory_order_release);
    }
    
    return len;
}

size_t RingBuffer::size() const
{
    size_t wp =  header_.write_pos_.load(std::memory_order_acquire);
    size_t rp =  header_.read_pos_.load(std::memory_order_acquire);
    if (wp >= rp)
    {
        return wp - rp;
    }
    else
    { 
        return header_.capacity_ - wp + rp;
    }
}


} // namespace alt