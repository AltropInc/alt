#pragma once

#include "SocketAddress.h"              // for SocketAddress

#include <util/Defs.h>                  // for ALT_UTIL_PUBLIC
#include <util/types/Enum.h>            // for Enum
#include <util/types/EnumSet.h>         // for EnumSet
#include <util/types/Clock.h>           // for Clock
#include <util/types/TemplateHelper.h>  // for MoveOnly
#include <util/system/OS.h>             // for SocketId

#include <algorithm>                    // for swap

namespace alt {

/// \brief Commonly used socket flags
ENUM (SocketFlag, uint8_t,
    KeepAlive,
    ReuseAddr,
    ReusePort,
    OOBInline,
    Broadcast,
    NonBlock,
    TCP_NoDelay,
    IP_DontFragment,
    IP_Multicast_Loop
    );
using SocketFlagS = EnumSet<SocketFlag>;

/// \brief Socket type
ENUM (SocketType, uint8_t,
    UNKNOWN,    ///< unknown
    STREAM,     ///< sequenced, reliable, two-way, connection-based byte streams
    DGRAM,      ///< datagrams of connectionless, unreliable messages of a fixed maximum length
    SEQPACKET,  ///< a sequenced, reliable, two-way connection based data transmission
                ///  path for datagrams of fixed maximum length
    RAW,        ///< raw network protocol access
    RDM         ///< a reliable datagram layer that does not
    );

/**
 * \struct Socket
 * \ingroup NetUtil
 * \brief Encapsulates a socket, a base class for all kinds of other sockets.
 */
class ALT_UTIL_PUBLIC Socket
{
  public:

    MOVEONLY(Socket);

    /// \brief Constructs a socket uninitialized with invalid socket address
    Socket();

    /// \brief Constructs a socket initialized for the given IP address family
    /// \param type SOCK_STREAM or SOCK_DGRAM
    Socket(IPFamily family, int type, int protocol=IPPROTO_IP);

    /// \brief Constructs a socket initialized for the given IP address family
    /// \param type socket type
    Socket(IPFamily family, SocketType type, int protocol=IPPROTO_IP);

    ~Socket();

    // BSD-style socket creation
    void open (int family, int type, int protocol=IPPROTO_IP) noexcept(false);
    void ensureOpen (int family, int type, int protocol=IPPROTO_IP) noexcept(false);
    void close ();

    //--------------------------------------------------------------------------------
    // Set and get socket options
    //--------------------------------------------------------------------------------

    /// \brief a wrapper to POSIX raw setsockopt
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id such as SO_LINGER, SO_KEEPALIVE, etc.
    /// \param optval pointer to option value
    /// \param optlen size of option value
    void setOption(int level, int option, const void* optval, socklen_t optlen) noexcept(false);

    /// \brief sets a socket option in generic type
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id such as SO_LINGER, SO_KEEPALIVE, etc.
    /// \param optval the option value
    template <typename OptValueT>
    void setOption(int level, int option, const OptValueT& optval) noexcept(false)
    {
        setOption(level, option, &optval, sizeof(OptValueT));
    }

    /// \brief sets socket option in IPAddress
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id that take ip address
    /// \param ip_addr the ip address
    void setOption(int level, int option, const IPAddress& ip_addr) noexcept(false);

    /// \brief sets a socket option in boolen flag
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id that take ip address
    /// \param enable the boolen enable/disable flag
    void setOption(int level, int option, bool enable) noexcept(false);

    /// \brief sets socket options on a given set of ommon boolen flags
    /// \param flags set of flags
    /// \param enable the boolen enable/disable value for all given flags
    void setOption(SocketFlagS flags, bool enable) noexcept(false);

    void setOption(SocketFlag flag, bool enable) noexcept(false);

    /// \brief a wrapper to POSIX raw getsockopt
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id such as SO_LINGER, SO_KEEPALIVE, etc.
    /// \param optval pointer to option value
    /// \param optlen size of option value
    void getOption (int level, int option, void *optval, socklen_t& optlen) const noexcept(false);

    /// \brief gets socket option in generic type
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id such as SO_LINGER, SO_KEEPALIVE, etc.
    /// \return the option value
    template <typename OptValueT>
    OptValueT getOption(int level, int option) const noexcept(false)
    {
        socklen_t optlen = sizeof(OptValueT);
        OptValueT optval;
        getOption(level, option, &optval, optlen);
        return optval;
    }

    /// \brief gets socket option in IPAddress
    /// \param level protocol level such as SOL_SOCKET and IPPROTO_TCP
    /// \param option option id such as SO_LINGER, SO_KEEPALIVE, etc.
    /// \param ip_addr output param for the ip addresse
    void getOption(int level, int option, IPAddress& ip_addr) const noexcept(false);

    /// \brief gets socket type in POSIX integer such as SOCK_STREAM, SOCK_DGRAM, etc
    int getRawSoketType();

    /// \brief gets socket type in SocketType enum value
    SocketType getSoketType();

    /// \brief check if the given flag is enabled
    /// \param flag the flag to check
    /// \return true if the flag is enabled, false otherwise
    bool isEnabled (SocketFlag flag) const noexcept(false);

    /// \brief Enable/disable linger. When enabled, a close or shutdown will not
    /// return until all queued messages for the socket have been successfully sent
    /// or the linger timeout has been reached.
    /// \param enable Enable linger when true, disable when false
    /// \param timeout_in_sec how many seconds to linger for
	void setLinger(bool enable, int timeout_in_sec) noexcept(false);

    /// \brief gets socket linger option values
	void getLinger(bool& enabled, int& timeout_in_sec) const noexcept(false);

    /// \brief sets IP time-to-live value for sending multicast datagrams
    /// \param ttl value:
    /// 0 - restricted to the same host
    /// 1 - restricted to the same subnet, this is the default
    /// 32 - restricted to the same site
    /// 64 - restricted to the same region
    /// 128 - restricted to the same continent
    /// 255 - unrestricted in scope
    void setTimeToLive(uint32_t ttl) noexcept(false);
 
    /// \brief gets IP time-to-live value for sending multicast datagrams
    uint32_t getTimeToLive() const noexcept(false);

	/// \brief sets sending buffer size.
	void setSendBufferSize(int size);

    /// \brief gets sending buffer size.
	int getSendBufferSize();

	/// \brief sets receiving buffer size.
	void setReceiveBufferSize(int size);

    /// \brief gets receiving buffer size.
	int getReceiveBufferSize();

    /// \brief sets the send timeout.
    /// \briefReturns the send timeout for the socket.
	void setSendTimeout(Clock::tick_type timeout);

    /// \brief gets the send timeout.
	Clock::duration getSendTimeout();

    /// \brief sets the receive timeout.
    void setReceiveTimeout(Clock::tick_type timeout);

    /// \brief gets the receive timeout.
    Clock::duration getReceiveTimeout();

    /// returns the current address to which the socket sockfd is bound
    SocketAddress address();
    SocketAddress peerAddress();

    /// \brief sends the data in the given buffer
    ssize_t send(const void *buffer, int length, int flags);

    /// \brief sends the data in the given iovec
    ssize_t send(const struct iovec *iov, int iovcnt, int flags);

    /// \brief sends message data in T
    template <typename T>
    ssize_t send(const T& msg, int flags=0)
    {
        return send(&msg, sizeof(msg), flags);
    }

    /// \brief sends the data in the given buffer to the given destination
    ssize_t sendTo(const void* buffer,
        int length,
        const SocketAddress& addr,
        int flags=0);

    /// \brief sends the data in the given iovec to the given destination
    ssize_t sendTo(const struct iovec *iov,
        int iovcnt,
        const SocketAddress& addr,
        int flags=0);

    /// \brief sends message data in T to the given destination
    template <typename T>
    ssize_t sendTo(const T& msg, const SocketAddress& addr, int flags=0)
    {
        return send(&msg, sizeof(msg), flags);
    }

    ssize_t receive(void* buffer, int length, int flags=0);
    ssize_t receive(struct iovec *iov, int iovcnt, int flags=0);

    ssize_t recvFrom(void* buffer,
        int length,
        const SocketAddress& addr,
        int flags);

    ssize_t recvFrom(struct iovec *iov,
        int iovcnt,
        const SocketAddress& addr,
        int flags);

    ssize_t available();

    /// \brief disable further transimission
    void shutdownSend ();

    /// \brief disable further data receiving
    void shutdownRecv ();

    /// \brief disable further transimission and data receiving
    void shutdown ();

    /*
    int joinGroup(uint32_t grpaddr, uint32_t iaddr);
    int leaveGroup(uint32_t grpaddr, uint32_t iaddr);

    // Join a multicast group and allow receiving data only from a
    // specified source.
    int joinSrcGroup(uint32_t grpaddr, uint32_t srcaddr, uint32_t iaddr);
    int leaveSrcGroup(uint32_t grpaddr, uint32_t srcaddr, uint32_t iaddr);
    */

    SocketId socketId() const { return fd_; }
    bool initilized () const { return fd_!=INVALID_SOCK_ID; }

    void swap (Socket& other) noexcept { std::swap(fd_, other.fd_); }

    struct scopedEnsure
    {
        scopedEnsure(Socket& socket, SocketFlag flag, bool enable)
            : socket_(socket), flag_(flag)
        {
            old_enabled_status_ = socket.isEnabled(flag);
            if (old_enabled_status_!=enable)
            {
                socket.setOption(flag, enable);
                changed_=true;
            }
        }
        ~scopedEnsure()
        {
            if (changed_)
            {
                socket_.setOption(flag_, old_enabled_status_);
            }
        }

    private:
        Socket& socket_;
        SocketFlag flag_;
        bool old_enabled_status_;
        bool changed_{false};
    };

  protected:

    // This following socket operations can be used in derived classes for various purposes

    /// \brief connect to TCP server of the given address
    /// \return 0 if the connection is established; non zero otherwise 
    int connect(const SocketAddress& address);

    /// \brief accept the connection from a TCP client
    /// \return a new TCP socket with the connection
    Socket* accept();

    /// \brief assigns the local end's address to the socket.
    /// \note This is typically for a server socket. For a client socket, however,
    /// the local address and port is normally not of importance unless the server restricts
    /// its clients to a certain range of port number, then bind() can be used on the client
    /// side as well. Binding is not only used for TCP sockets, but aslo for UDP and other
    /// protocols as well.
    void bind(const SocketAddress& addr);

    /// \brief listen for connections on a socket
    /// \param backlog maximum length to which the queue of pending connections
    /// for this socket may grow.
    void listen(int backlog);

    friend class StreamConnection;
    Socket(SocketId fd);

	SocketId         fd_ {INVALID_SOCK_ID};
};

#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
// In Windwos, the WSAStartup function must be the first Windows Sockets
// function called by an application or DLL. It allows an application or
// DLL to specify the version of Windows Sockets required and retrieve
// details of the specific Windows Sockets implementation. The application
// or DLL can only issue further Windows Sockets functions after successfully
// calling WSAStartup.
int SockAPIInit();
void SockAPICleanup() { WSACleanup(); }
#else
// for non windwos OS
ALT_INLINE constexpr int SockAPIInit() { return 0; }
ALT_INLINE constexpr void SockAPICleanup() { }
ALT_INLINE int closesocket(SocketId fd) { return ::close(fd); }
#endif

} // namespace alt

