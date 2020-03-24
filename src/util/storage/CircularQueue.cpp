#include "CircularQueue.h"
//#include "Logger.h"
#include <util/numeric/Intrinsics.h>

namespace alt {


CircularQueue::QueueHeader::QueueHeader(size_t entry_size, size_t entry_number)
  : entry_number_( power2Next<size_t>(entry_number))
  , entry_size_(requiredEntrySize(entry_size))
  , entry_size_shift_ (log2Ceil(entry_size_))
  , entry_size_mask_ (entry_size_-1)
{
    entry_buffer_ = (char*)::malloc(entry_size_*entry_number_);
    owns_buffer_ = true;
}

CircularQueue::QueueHeader::QueueHeader(size_t entry_size, size_t entry_number, char* buffer)
  : entry_buffer_(buffer)
  , entry_number_( power2Next<size_t>(entry_number))
  , entry_size_(requiredEntrySize(entry_size))
  , entry_size_shift_ (log2Ceil(entry_size_))
  , entry_size_mask_ (entry_size_-1)
{
}

CircularQueue::QueueHeader::~QueueHeader()
{
    if (owns_buffer_) ::free(entry_buffer_);
}

CircularQueue::CircularQueue(size_t entry_size, size_t entry_number, char* buffer)
  : header_(entry_size, entry_number, buffer)
{
}

CircularQueue::CircularQueue(size_t entry_size, size_t entry_number)
  : header_(entry_size, entry_number)
{
}

size_t CircularQueue::requiredEntrySize(size_t entry_size)
{
    size_t entry_sz = power2Next<size_t>(
            std::max(sizeof(EntryHeader) + entry_size,
                     SysConfig::instance().cache_line_size_));
}

size_t CircularQueue::requiredHeaderSize()
{
    return constAlign(sizeof(QueueHeader), SysConfig::instance().cache_line_size_);
}

size_t CircularQueue::requiredSize(size_t entry_size, size_t entry_number)
{
    return requiredHeaderSize() +
           requiredEntrySize(entry_size) * power2Next<size_t>(entry_number);
}

CircularQueue* CircularQueue::create(char* addr,  const MemoryAttrs& attrs,
    size_t entry_size, size_t entry_number)
{
    CircularQueue* queue(nullptr);
    if (attrs.is_new_)
    {
        queue = new (addr) CircularQueue(entry_size, entry_number, addr+requiredHeaderSize());
    }
    else
    {
        queue = reinterpret_cast<CircularQueue*>(addr);
    }
    return queue;
}

CircularQueue::EntryHeader* CircularQueue::getNextWriteEntry()
{ 
    int64_t seq = header_.write_sn_.acquire();
    EntryHeader& entry_header = (*this)[seq];
    entry_header.sequence_ = seq;
    return &entry_header;
}

CircularQueue::EntryHeader* CircularQueue::getNextNWriteEntry(int num)
{
    int64_t seq = header_.write_sn_.acquire(num);
    for (int64_t s=seq; s<=seq+num; ++s)
    {
        EntryHeader& entry_header = (*this)[s];
        entry_header.sequence_ = s;
    }
    return &(*this)[seq];
}

const CircularQueue::EntryHeader* CircularQueue::read()
{
    // get current read sequence
    int64_t read_seq = header_.read_sn_.acquire(*this);
    if (read_seq < 0)
    {
        return nullptr;
    }
    auto & entry = (*this)[read_seq];
    if (entry.sequence_ > read_seq)
    {
        ++header_.overrun_cnt_;
        return nullptr;
    }
    return &entry;
}

const CircularQueue::EntryHeader* CircularQueue::read (int64_t read_seq)
{
    auto & entry = (*this)[read_seq];
    if (!entry.isValid())
    {
        return nullptr;
    }
    if (entry.sequence_ > read_seq)
    {
        ++header_.overrun_cnt_;
        return nullptr;
    }
    return &entry;
}

}
