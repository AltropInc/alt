#pragma once

#include "Socket.h"              // for Socket

namespace alt {


/**
 * \struct SocketStream
 * \ingroup NetUtil
 * \brief Encapsulates a TCP stream socket
 */
class ALT_UTIL_PUBLIC SocketStream: public Socket
{
public:

    MOVEONLY(SocketStream);

    /// \brief Constructs an uninitialized stream socket
    SocketStream() {}

    /// \brief Constructs a stream socket and connect to the given address
    explicit SocketStream(const SocketAddress& address);

    /// \brief Constructs an uninitialized stream socket for the given IP family
    SocketStream(IPFamily family) : Socket(family, SOCK_STREAM) {};

    /// \brief destructor
	virtual ~SocketStream() {};

    void open (int family) noexcept(false) { Socket::open(family, SOCK_STREAM); }

    /// \brief connects to TCP server of the given address
    /// \return 0 if the connection is established; non zero otherwise 
	int connect(const SocketAddress& addr) { return Socket::connect(addr); }

    /// \brief binds local address. See comments in Socket::bind
	void bind(const SocketAddress& addr) { Socket::bind(addr); };
	
	//void sendUrgent(unsigned char data);
		/// Sends one byte of urgent data through
		/// the socket.
		///
		/// The data is sent with the MSG_OOB flag.
		///
		/// The preferred way for a socket to receive urgent data
		/// is by enabling the SO_OOBINLINE option.

  private:

    /// \brief Constructs a stream socket from the socket created by the underlying os,
    /// for instance, a socket by accepting a connection
    SocketStream(SocketId fd) : Socket(fd) {}

	enum
	{
		BUFFER_SIZE = 1024
	};

    friend class Socket;

	friend class ServerSocket;
	friend class SocketIOS;
};

} // namespace alt