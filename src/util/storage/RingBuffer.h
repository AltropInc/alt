#pragma once
#include <util/system/Platform.h>
#include <util/numeric/Intrinsics.h>
#include <util/ipc/SharedMemory.h>
#include <util/system/SysConfig.h>
#include <sys/uio.h>
#include <atomic>
#include <stdlib.h>
#include <cstring>
//#include <iostream>

namespace alt {

/**
 * \class RingBuffer
 * \brief implements a lock-free circular buffer for single writer/single reader
 */
class ALT_UTIL_PUBLIC RingBuffer
{
  public:

    /// \brief Constructor
    /// \param buffer  buffer for the ring
    /// \param buffer_sz  size of the buffer
    /// \note Use this constructor to pass the buffer allocated outside
    RingBuffer(char* buffer, size_t buffer_sz);

    /// \brief Constructor
    /// \param buffer_sz  size of the buffer
    /// \note Use this constructor to create buffer internally
    RingBuffer(size_t buffer_sz);

    /// \brief Empty constructor. setBuffer must be called to set the buffer
    RingBuffer();

    /// \brief Destructor
    /// \note the writer and reader threads must have stopped using the buffer before the
    /// destructor can be safely called.
    ~RingBuffer();

    /// \brief Get buffer header size
    static size_t requiredHeaderSize();

    /// \brief Get required memory size for both buffer header and the content buffer
    /// \param capacity the size of the content buffer
    /// \note This function is required by SharedMemory template
    static size_t requiredSize(size_t capacity);

    /// \brief Create a ring buffer instance in an allocated memory
    /// \param addr the address of the memory
    /// \param attrs memory addtributes
    /// \param buffer_sz the size of the content buffer
    /// \note if attrs.is_new_ is true, a RingBuffer's constructor will be called to
    /// create a new instance. Otherwise, addr contains an instance already created
    /// by other process and this function simply returns the instance for sharing. 
    static RingBuffer* create (char* addr, const MemoryAttrs& attrs, size_t buffer_sz);

    /// \brief Set the buffer
    /// \param buffer  buffer for the ring
    /// \param buffer_sz  size of the buffer
    /// \note This must be called if empty constructor is used
    void setBuffer(char* buffer, size_t buffer_sz);

    /// \brief write n bytes into the buffer
    /// \param bytes buffer contains the bytes to be written
    /// \param n number of bytes to be written
    /// \param split tells whether the data should be split in wrap case
    /// \return true if the bytes are written successfully; false if buffer is full
    /// \note nothing will happen if the buffer has no sufficient space to hold the bytes
	bool write (const char* bytes, size_t n, bool split=true);

    /// \brief fetch all unread bytes in the buffer into iov
    /// \param iov io vector to give the positions and length in the ring buffer
    /// \return the length of the unread bytes given in io vector
    /// \note fetched bytes are not copied. io vector conatins positions only. commitRead
    /// must be called after the fetched data are processed to free space for the writer.
	size_t fetchAll(struct iovec* iov);

    /// \brief fetch n unread bytes in the buffer into iov
    /// \param iov io vector to give the positions and length in the ring buffer
    /// \param n number of bytes to fetch
    /// \return the length of the unread bytes given in io vector. zero if the buffer
    /// contains bytes less than n
    /// \note fetched bytes are not copied. io vector conatins positions only. commitRead
    /// must be called after the fetched data are processed to free space for the writer.
	size_t fetch(struct iovec* iov,  size_t n);

    /// \brief continue to fetch n unread and unfetched bytes in the buffer into iov
    /// \param iov io vector to give the positions and length in the ring buffer
    /// \param n number of bytes to fetch
    /// \return the length of the bytes given in io vector. zero if the buffer
    /// contains bytes less than n
    /// \note commitRead must be called after the all fetchs are done.
    size_t fetchNext(struct iovec* iov,  size_t n);

    /// \brief commit read after fetch. Read position will be advanced to discard commited bytes.
    /// \param uncommited numberof bytes uncommited in fetch
    void commitRead(size_t uncommited);

    /// \brief copy n bytes in the buffer into buf. Read position will be advanced to discard
    /// copied bytyes
    /// \param buf buffer to accept data
    /// \param n number of bytes to read
    /// \return the length of the read bytes copied into the buffer. zero if the buffer
    /// contains bytes less than n
	size_t read(char* buf, size_t n);

    /// \brief get free space for write
    /// \param iov vector to holds the free spaces in the buffer. If the buffer is empty,
    /// both write_pos_ and read_pos_ will be reset to 0.
    size_t fetchFreeSpace(struct iovec* iov);

    /// \brief commite the written bytes after calling fetchFreeSpace
    /// \param commited number of bytes written.
    void commitWrite(size_t commited);

    /// \brief returns number of unread bytes written in the buffer
    size_t size() const;

    /// \brief returns the capacity of the buffer
    size_t capacity() const { return header_.capacity_; }

    /// \brief check free space for write
    bool hasFreeSpace(size_t required_non_split_space, size_t required_space) const;

    /// \return true of the buffer is empty
    bool empty() const;

  protected:
    RingBuffer(RingBuffer &) = delete;
    RingBuffer(RingBuffer &&) = delete;
    RingBuffer& operator =(RingBuffer const &) = delete;
    RingBuffer& operator =(RingBuffer const &&) = delete;
    size_t fetch_i(struct iovec* iov,  size_t len, size_t read_pos);

    // In shared memeory, the buffer header and the buffer must be allocated
    // in the same memoery block.
    struct BufferHeader
    {
        const bool              owns_buffer_  {false};
        size_t    			    capacity_     {0};
        size_t                  mod_mask_     {0};
        char*       			buffer_       {nullptr};
        uint64_t                commit_pos_   {0};

        CACHE_LINE_ALIGN std::atomic<uint64_t>  read_pos_     {0};
        CACHE_LINE_ALIGN std::atomic<uint64_t>	write_pos_    {0};
        std::atomic<uint64_t>	                wasted_space_ {0};

        BufferHeader(char* buffer, size_t buffer_sz);
        BufferHeader(size_t buffer_sz);
        BufferHeader() = default;
    }
    header_;
};

/**
 * \class RingStreamBuffer
 * \brief implements a lock-free circular message buffer for single writer/single reader
 * \tparam MsgSizeType Message length type.
 * \note Each message is placed in the buffer and prefixed with an integer in MsgSizeType
 *  to indicate the length of the message
 */
template<typename MsgSizeType>
class RingMsgBuffer: public RingBuffer
{
  public:
    
    using RingBuffer::RingBuffer;

    /// \brief write content in stream buffer into the buffer
    /// \param stream_buffer the stream buffer
    /// \param length length of the content contained in the stream buffer
    bool write(const char* stream_buffer, MsgSizeType length)
    {
        //std::cout << "BEFORE Write " << stream_buffer << ": read_pos=" << header_.read_pos_.load()
        //<< "write_pos=" << header_.write_pos_.load() << std::endl;
        if (!hasFreeSpace(sizeof(MsgSizeType), length))
        {
            return false;
        }
        // do not split length
        RingBuffer::write (reinterpret_cast<char*>(&length), sizeof(MsgSizeType), false);
        // payload can be split
        RingBuffer::write (stream_buffer, size_t(length), true);
        //std::cout << "AFTER Write: read_pos=" << header_.read_pos_.load()
        //<< "write_pos=" << header_.write_pos_.load() << std::endl;
        return true;
    }

    /// \brief write payload in iovec into the buffer
    /// \param iov the iovec containing the payload
    /// \param vec_num number of entries in iov
    /// \param total_length total length of payload
    bool write(struct iovec* iov, size_t vec_num, MsgSizeType total_length)
    {
        if (!hasFreeSpace(sizeof(MsgSizeType), total_length))
        {
            return false;
        }
        RingBuffer::write (reinterpret_cast<char*>(&total_length), sizeof(MsgSizeType), false);
        for (size_t i=0; i<vec_num; ++i)
        {
            RingBuffer::write (reinterpret_cast<char*>(iov[i].iov_base), iov[i].iov_len, true);
        }
    }

    /// \brief read a message
    /// \param payload the buffer for the payload. Must be big enough to accept largest
    /// stream message used
    /// \return number of bytes read if the complete message is copied into the payload;
    /// zero if the buffer contains bytes less than the size the complete message
    MsgSizeType read(char* payload)
    {
        struct iovec iov[2];
        size_t len = RingBuffer::fetch(iov, sizeof(MsgSizeType));
        if (len==0) return 0;
        size_t payload_len = *(reinterpret_cast<MsgSizeType*>(iov[0].iov_base));
        payload_len = RingBuffer::fetchNext(iov, payload_len);
        if (payload_len==0) return 0;
        memcpy(payload, iov[0].iov_base, iov[0].iov_len);
        if (ALT_UNLIKELY(iov[1].iov_len))
        {
            memcpy(payload+iov[0].iov_len, iov[1].iov_base, iov[1].iov_len);
        }
        commitRead(0);
        return payload_len;
    }

    /// \brief fetch a message, zero copy read
    /// \return number of bytes fetched if the complete message is loacted by iov;
    /// zero if the buffer contains bytes less than the size the complete message.
    /// \note commitRead must be called after the fetched data are processed to free
    ///space for the writer.
    size_t fetch(struct iovec* iov)
    {
        struct iovec length_iov[2];
        size_t len = RingBuffer::fetch(length_iov, sizeof(MsgSizeType));
        if (len==0) return 0;
        size_t msg_length = *(reinterpret_cast<MsgSizeType*>(length_iov[0].iov_base));
        return RingBuffer::fetchNext(iov, msg_length);
    }
};

/**
 * \class RingTypedMsgBuffer
 * \brief implements a single writer/single reader, lock-free circular buffer for
 * nessages with a header to for type id.
 * \tparam MsgHeaderT Message Header type. It must be packed struct that follows the example
 * as shown below:
 *      struct __attribute__ ((packed)) MessageHeader
 *      {
 *          using MsgSizeType = uint16_t;  // message size type
 *          MsgSizeType length() { return length_; }
 *          MsgSizeType         length_;   // header: payload length, excluding this field
 *                                         // LengthPayloadOnly is true
 *      };
 * \tparam LengthPayloadOnly True if the length in header is for payload only, otherwise
 * the length is the size of the message structire including the length field
 */
template<typename MsgHeaderT, bool LengthPayloadOnly =true>
class RingTypedMsgBuffer: public RingBuffer
{
  public:
    using RingBuffer::RingBuffer;
    using MsgSizeType = typename MsgHeaderT::MsgSizeType;

    /// \brief write a message into the buffer
    /// \param msg the message to be written
    bool write(const MsgHeaderT* msg)
    {
        // do not split data on wrap
        return RingBuffer::write (
            msg,
            LengthPayloadOnly ? msg->length()+sizeof(MsgSizeType) : msg->length(),
            false);
    }

    /// \brief read a message
    /// \param msg_buffer the buffer for the message. Must be big enough to accept largest
    /// message used
    /// \return true if the complete message is copied into the buffer. false if the buffer
    /// contains bytes less than the size the message
    bool read(MsgSizeType& msg_length, char* payload)
    {
        struct iovec iov[2];
        size_t len = RingBuffer::fetch(iov, sizeof(MsgSizeType));
        if (len==0) return false;
        msg_length = *(reinterpret_cast<MsgSizeType*>(iov[0].iov_base));
        if (!LengthPayloadOnly)
        {
            msg_length -= sizeof(MsgSizeType);
        }
        size_t payload_len = RingBuffer::fetchNext(iov, msg_length);
        if (payload_len==0) return false;
        memcpy(payload, iov[0].iov_base, iov[0].iov_len);
        commitRead(0);
        return true;
    }

    /// \brief fetch a message, zero copy read
    /// \return pointer to message header in the buffer if the message is complete
    /// \note commitRead must be called after the fetched data are processed to free
    ///space for the writer.
    MsgHeaderT* fetch()
    {
        struct iovec iov[2];
        size_t len = RingBuffer::fetch(iov, sizeof(MsgSizeType));
        if (len==0) return nullptr;
        MsgSizeType msg_length;
        msg_length = *(reinterpret_cast<MsgSizeType*>(iov[0].iov_base));
        if (!LengthPayloadOnly)
        {
            msg_length -= sizeof(MsgSizeType);
        }
        struct iovec payload_iov[2];
        size_t payload_len = RingBuffer::fetchNext(payload_iov, msg_length);
        if (payload_len==0) return nullptr;
        return iov[0].iov_base;
    }
};

using SharedRingBuffer = SharedContainer<SharedMemory, RingBuffer>;
template<typename MsgSizeType> using SharedRingMsgBuffer
    = SharedContainer<SharedMemory, RingMsgBuffer<MsgSizeType>>;
template<typename MsgHeaderT> using SharedRingTypedMsgBuffer
    = SharedContainer<SharedMemory, RingTypedMsgBuffer<MsgHeaderT>>;
}