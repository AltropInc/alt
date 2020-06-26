#pragma once

#include "Socket.h"                         // for Socket
#include <util/storage/RingBuffer.h>        // for RingBuffer
#include <util/system/EventPoller.h>     // for EventHandler

namespace alt {

class StreamListener
{
  public:
    virtual void onStreamData(RingBuffer& data);
};

/**
 * \struct StreamConnection
 * \ingroup NetUtil
 * \brief Encapsulates a connection between two sockets for transmitting data streams.
 */
class ALT_UTIL_PUBLIC StreamConnection: public FDEventHandler
{
  public:

    StreamConnection(
        StreamListener& listener,
        size_t send_buffer_size,
        size_t recv_buffer_size,
        FDEventPoller * poll);

    StreamConnection(
        FdId     fd,
        StreamListener& listener,
        size_t send_buffer_size,
        size_t recv_buffer_size,
        FDEventPoller * poll);

    virtual ~StreamConnection();

    void connect(const SocketAddress& address);
    void disconnect();

    /// \brief sends the data in the given buffer
    void send(const char *buffer, ssize_t length);

    /// \brief sends the data in the given iovec
    void send(const struct iovec *iov, int iovcnt);

    /// \brief sends message data in T
    template <typename T>
    void send(const T& msg)
    {
        return send(&msg, sizeof(msg));
    }

    FdId fd() const override { return socket_.socketId(); }

    FDEventIdSet onEvent(Clock::tick_type tick_realtime, FDEventIdSet event_ids) override;

  private:

    void receive(Clock::tick_type tick_realtime);

    void sendDirect(const char *buffer, ssize_t length);
    void bufferSendData(const char *buffer, ssize_t length);
    void flushSendBuffer();

    StreamListener&   listener_;
    RingBuffer        send_buffer_;
    RingBuffer        recv_buffer_;
    Socket            socket_;
    FDEventPoller *   poll_{nullptr};
    bool              connected_ {false};
};


} // namespace alt