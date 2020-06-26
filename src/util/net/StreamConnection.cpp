#include "StreamConnection.h"           // for StreamConnection, this calss

#include <util/system/SysError.h>       // for SYS_ERROR_LOG

#include <assert.h>                     // for assert

namespace alt {

StreamConnection::StreamConnection(
    StreamListener& listener,
    size_t send_buffer_size,
    size_t recv_buffer_size,
    FDEventPoller * poll)
    : listener_(listener)
    , send_buffer_(send_buffer_size)
    , recv_buffer_(recv_buffer_size)
    , poll_(poll)
{
}

StreamConnection::StreamConnection(
    FdId  fd,
    StreamListener& listener,
    size_t send_buffer_size,
    size_t recv_buffer_size,
    FDEventPoller * poll)
    : listener_(listener)
    , send_buffer_(send_buffer_size)
    , recv_buffer_(recv_buffer_size)
    , socket_(fd)
    , poll_(poll)
{
    poll_->book(this, FDEventId::EVENT_IN);
    connected_ = true;
}

void StreamConnection::connect(const SocketAddress& address)
{
    if (connected_)
    {
        SYS_ERR_THROW(NetException, "Already connected", false);
    }
    int res = socket_.connect(address);
    if (res==0)
    {
        poll_->book(this, FDEventId::EVENT_IN);
        connected_ = true;
    }
    if (res==EAGAIN)
    {
        // try again later
    }   
}


void StreamConnection::disconnect()
{
    if (connected_)
    {
        socket_.close();
        connected_ = false;
    }
}

StreamConnection::~StreamConnection()
{
    if (poll_)
    {
        poll_->remove(this);
    }    
}

inline void StreamConnection::bufferSendData(const char *buffer, ssize_t length)
{
    bool buffer_empty_before = send_buffer_.empty();
    if (!send_buffer_.write(buffer, length))
    {
        SYS_ERR_THROW(NetException, "StreamConnection send failed. Buffer is full", false);
    }
    if (poll_ && !send_buffer_.empty() && buffer_empty_before)
    {
        // book EVENT_OUT in poll so we'll get notified when we can send again
        poll_->book(this, FDEventId::EVENT_OUT);
    }
}

inline void StreamConnection::sendDirect(const char *buffer, ssize_t length)
{
    ssize_t bytes_sent = socket_.send(buffer, length);
    if (bytes_sent < length)
    {
        bufferSendData(buffer + bytes_sent, length - bytes_sent);
    }
}

void StreamConnection::flushSendBuffer()
{
    iovec iov[2];
    size_t data_size = send_buffer_.fetchAll(iov);   // zero copy fetch
    if (data_size)
    {
        ssize_t bytes_sent = socket_.send(iov, iov[1].iov_len ? 2 : 1);
        assert(data_size >= bytes_sent);
        send_buffer_.commitRead(data_size - bytes_sent /* uncommitted */);
    }
}

void StreamConnection::send(const char *buffer, ssize_t length)
{
    if (send_buffer_.empty())
    {
        // send data to socket directly if the buffer is empty
        sendDirect(buffer, length);
    }
    else
    {
        // flush the send buffer first, the try to send data directly
        flushSendBuffer();
        if (send_buffer_.empty())
        {
            sendDirect(buffer, length);
        }
        else
        {
            bufferSendData(buffer, length);
        }
    }
}

void StreamConnection::receive(Clock::tick_type tick_realtime)
{
    iovec iov[2];
    ssize_t bytes_got;
    do
    {
        size_t buffer_size = recv_buffer_.fetchFreeSpace(iov);
        if (iov[1].iov_len==0)
        {
            bytes_got = socket_.receive(iov[0].iov_base, (iov[0].iov_len));
        }
        else
        {
            bytes_got = socket_.receive(iov, 2);
        }
        recv_buffer_.commitWrite(bytes_got);
        listener_.onStreamData(recv_buffer_);
    }
    while (bytes_got > 0);
}


FDEventIdSet StreamConnection::onEvent(Clock::tick_type tick_realtime, FDEventIdSet event_ids)
{
    FDEventIdSet done_set;
    if (event_ids.has(FDEventId::EVENT_OUT))
    {
        flushSendBuffer();
        if (send_buffer_.empty())
        {
            // no longer interested in EVENT_OUT
            done_set += FDEventId::EVENT_OUT;
        }
    }
    else if (event_ids.has(FDEventId::EVENT_IN))
    {
        receive(tick_realtime);
    }
    return done_set;
}

} // namespace alt
