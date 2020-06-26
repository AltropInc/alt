

#include "Socket.h"                     // for Socket, this class

#include <util/numeric/Intrinsics.h>    // for htobe
#include <util/system/SysError.h>       // for SYS_ERROR_LOG
#include <util/string/StrPrint.h>       // for StrPrinter

#include <sys/ioctl.h>                  // for ioctl
#include <fcntl.h>                      // for fcntl
#include <assert.h>                     // for assert

#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
#define SHUT_RD     SD_RECEIVE
#define SHUT_WR     SD_SEND
#define SHUT_RDWR   SD_BOTH
#endif

namespace alt {

using TimeSpan  = std::chrono::duration<int, std::nano>;
using TimePoint = std::chrono::duration<int, std::nano>;

Socket::Socket() : fd_ {INVALID_SOCK_ID}
{}

Socket::Socket(SocketId fd)
    : fd_ {fd}
{}

Socket::Socket(IPFamily family, int type, int protocol)
{
#if ALT_IPV6_AVAILABLE
    if (family == IPFamily::IPv6)
    {
        open (AF_INET6, type, protocol);
    }
#endif
    open (AF_INET, type, protocol);
}

inline int SocketTypeToRawInt (SocketType type)
{
    switch (type)
    {
        case SocketType::STREAM : return SOCK_STREAM;
        case SocketType::DGRAM  : return SOCK_DGRAM;
        case SocketType::SEQPACKET : return SOCK_SEQPACKET;
        case SocketType::RAW : return SOCK_RAW;
        case SocketType::RDM : return SOCK_RDM;
        case SocketType::UNKNOWN : return SOCK_RAW;
    }  
    return SOCK_RAW;
}

Socket::Socket(IPFamily family, SocketType type, int protocol)
    : Socket(family, SocketTypeToRawInt(type), protocol)
{
}

Socket::~Socket()
{
	close();
}

void Socket::open (int family, int type, int protocol)
{
    assert (fd_==INVALID_SOCK_ID);

    if ((fd_ = ::socket(family, type, protocol)) < 0)
    {
        fd_ = INVALID_SOCK_ID;
        SYS_ERR_THROW(NetException);
    }
#if (ALT_OS_FAMILY==ALT_OS_FAMILY_MAC)
    // ignore SIGPIPE to make the default behavior like in other OS platforms
    setOption(SOL_SOCKET, SO_NOSIGPIPE, 1);
#endif
}

void Socket::ensureOpen (int family, int type, int protocol)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        open(family, type, protocol);
    }
}

void Socket::close ()
{
    if (fd_!=INVALID_SOCK_ID)
    {
        closesocket(fd_);
        fd_ = INVALID_SOCK_ID;
    }
}

//-----------------------------------------------------------------------------------------
// Set and get socket options
//-----------------------------------------------------------------------------------------
void Socket::setOption(int level, int option, const void* optval, socklen_t optlen)
{
    // Note: cast option_alue to char* to accommodate Windows interface
    if (::setsockopt(fd_, SOL_SOCKET, option, (const char*)optval, optlen) < 0)
    {
       SYS_ERR_THROW(NetException);
    }
}

void Socket::setOption(int level, int option, const IPAddress& ip_addr)
{
    setOption(level, option, ip_addr.addr(), ip_addr.addrLength());
}

void Socket::setOption(int level, int option, bool enable)
{
	int value = enable ? 1 : 0;
	setOption(level, option, value);
}

void Socket::setOption(SocketFlagS flags,  bool enable)
{
    for (auto flag: SocketFlag::enum_values)
    {
        if (flags.has(flag))
        {
            setOption(flag,  enable);
        }
    }
}

void Socket::setOption(SocketFlag flag,  bool enable)
{
	int value = enable ? 1 : 0;
    switch (flag)
    {
        case SocketFlag::KeepAlive:
            setOption(SOL_SOCKET, SO_KEEPALIVE, value);
            break;
        case SocketFlag::ReuseAddr:
            setOption(SOL_SOCKET, SO_REUSEADDR, value);
            break;
        case SocketFlag::ReusePort:
#if defined(SO_REUSEPORT)
            try {
                setOption(SOL_SOCKET, SO_REUSEPORT, value);
            }
            catch (NetException&) {
                // ignore if not available
            }
#endif
            break;

        case SocketFlag::OOBInline:
            setOption(SOL_SOCKET, SO_OOBINLINE, value);
            break;
        case SocketFlag::Broadcast:
            setOption(SOL_SOCKET, SO_BROADCAST, value);
            break;
        case SocketFlag::NonBlock:
            {
                int fh_flags = ::fcntl(fd_, F_GETFL);
                if (fh_flags < 0)
                {
                    SYS_ERR_THROW(NetException);
                }
                if (enable)
                {
                    setBits(fh_flags, O_NONBLOCK);
                }
                else
                {
                    clearBits(fh_flags, O_NONBLOCK);
                }
                if (::fcntl(fd_, F_SETFL,fh_flags) < 0)
                {   
                    SYS_ERR_THROW(NetException);;
                }                      
            }
            break;
        case SocketFlag::TCP_NoDelay:
            setOption(IPPROTO_TCP, TCP_NODELAY, value);
            break;
        case SocketFlag::IP_DontFragment:
#if   defined(IP_DONTFRAGMENT)
            setOption(IPPROTO_IP, IP_DONTFRAGMENT, value);
#elif defined(IP_MTU_DISCOVER)
            setOption(IPPROTO_IP, IP_MTU_DISCOVER,
                enable ? IP_PMTUDISC_DO : IP_PMTUDISC_DONT);
#elif defined(IP_DONTFRAG)
            setOption(IPPROTO_IP, IP_DONTFRAG, value);
#else
            SYS_ERR_THROW(NetException, "IP_DontFragment not implemented", false);
#endif
            break;
        case SocketFlag::IP_Multicast_Loop:
            setOption(IPPROTO_IP, IP_MULTICAST_LOOP, enable ? uint8_t(1) : uint8_t(0));
            break;
    }
}

void Socket::getOption (int level, int option, void *optval, socklen_t& optlen) const
{
    if (::getsockopt(fd_, SOL_SOCKET, option, (char*)optval, &optlen) < 0)
    {
       SYS_ERR_THROW(NetException);
    }
}

void Socket::getOption(int level, int option, IPAddress& ip_addr) const
{
    socklen_t len = sizeof(struct in6_addr);
    char buffer[sizeof(struct in6_addr)];
    getOption(level, option, (void*)buffer, len);
    ip_addr = IPAddress((void*)buffer, len);
}

int Socket::getRawSoketType()
{
    try {
        return getOption<int>(SOL_SOCKET,SO_TYPE);
    }
    catch (...) {
        return -1;
    }
}

SocketType Socket::getSoketType()
{
    int st= getRawSoketType();
    switch (st)
    {
        case SOCK_STREAM : return SocketType::STREAM;
        case SOCK_DGRAM : return SocketType::DGRAM;
        case SOCK_SEQPACKET : return SocketType::SEQPACKET;
        case SOCK_RAW : return SocketType::RAW;
        case SOCK_RDM : return SocketType::RDM;
    }
    return SocketType::UNKNOWN;
}

bool Socket::isEnabled(SocketFlag flag) const
{
    if (fd_ == INVALID_SOCK_ID)
    {
        return false;
    }
    switch (flag)
    {
        case SocketFlag::KeepAlive:
            return  getOption<int>(SOL_SOCKET, SO_KEEPALIVE) != 0;

        case SocketFlag::ReuseAddr:
            return  getOption<int>(SOL_SOCKET, SO_REUSEADDR) != 0;

        case SocketFlag::ReusePort:
#if defined(SO_REUSEPORT)
            try {
                return  getOption<int>(SOL_SOCKET, SO_REUSEPORT) != 0;
            }
            catch (NetException&) {}
#endif
            return false;

        case SocketFlag::OOBInline:
            return  getOption<int>(SOL_SOCKET, SO_OOBINLINE) != 0;

        case SocketFlag::Broadcast:
            return  getOption<int>(SOL_SOCKET, SO_BROADCAST) != 0;

        case SocketFlag::NonBlock:
            {
                int fh_flags = ::fcntl(fd_, F_GETFL);
                if (fh_flags < 0)
                {
                    SYS_ERR_THROW(NetException);
                }
                return (fh_flags & O_NONBLOCK)!=0;                        
            }
            break;
        case SocketFlag::TCP_NoDelay:
            return  getOption<int>(IPPROTO_TCP, TCP_NODELAY) != 0;

        case SocketFlag::IP_DontFragment:
#if   defined(IP_DONTFRAGMENT)
            return  getOption<int>(IPPROTO_IP, IP_DONTFRAGMENT) != 0;
#elif defined(IP_MTU_DISCOVER)
            return  getOption<int>(IPPROTO_IP, IP_MTU_DISCOVER) != IP_PMTUDISC_DONT;
#elif defined(IP_DONTFRAG)
            return  getOption<int>(IPPROTO_IP, IP_DONTFRAG) != 0;
#else
            SYS_ERR_THROW(NetException, "IP_DontFragment not implemented", false);
#endif
            break;

        case SocketFlag::IP_Multicast_Loop:
            return  getOption<int>(IPPROTO_IP, IP_MULTICAST_LOOP) != 0;
    }
    return false;
}

void Socket::setLinger(bool enable, int timeout_in_sec)
{
	setOption(SOL_SOCKET, SO_LINGER, linger{enable, uint16_t(timeout_in_sec)});
}

void Socket::getLinger(bool& enabled, int& timeout_in_sec) const
{
	struct linger linger_opt = getOption<linger>(SOL_SOCKET, SO_LINGER);
	enabled           = linger_opt.l_onoff != 0;
	timeout_in_sec    = linger_opt.l_linger;
}

void Socket::setTimeToLive(uint32_t ttl)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "setTimeToLive on invalid socket", false);
    }

#if ALT_IPV6_AVAILABLE
	struct sockaddr_storage addr_storage;
    socklen_t addr_sz = sizeof(addr_storage);
    if (::getsockname(fd_, (struct sockaddr*)&addr_storage, &addr_sz) < 0)
    {
        SYS_ERR_THROW(NetException);
    }

	if (addr_storage.ss_family == AF_INET6)
	{
        setOption(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
        return;
    }
#endif
    uint8_t ttl_u8 = (unsigned char) ttl;
    setOption(IPPROTO_IP, IP_MULTICAST_TTL, ttl_u8);
}

uint32_t Socket::getTimeToLive() const
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "getTimeToLive from invalid socket", false);
    }

#if ALT_IPV6_AVAILABLE
	struct sockaddr_storage addr_storage;
    socklen_t addr_sz = sizeof(addr_storage);
    if (::getsockname(fd_, (struct sockaddr*)&addr_storage, &addr_sz) < 0)
    {
        SYS_ERR_THROW(NetException);
    }
	if (addr_storage.ss_family == AF_INET6)
	{
        return getOption<uint32_t>(IPPROTO_IPV6, IPV6_MULTICAST_HOPS);
    }
#endif
    return getOption<uint8_t>(IPPROTO_IP, IP_MULTICAST_TTL);
}

void Socket::setSendBufferSize(int size)
{
	setOption(SOL_SOCKET, SO_SNDBUF, size);
}


int Socket::getSendBufferSize()
{
	return getOption<int>(SOL_SOCKET, SO_SNDBUF);
}


void Socket::setReceiveBufferSize(int size)
{
	setOption(SOL_SOCKET, SO_RCVBUF, size);
}

int Socket::getReceiveBufferSize()
{
	return getOption<int>(SOL_SOCKET, SO_RCVBUF);
}

void Socket::setSendTimeout(Clock::tick_type timeout)
{
#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
    // ms winsock timeout is in milliseconds
	int32_t ms = static_cast<int32_t>(Clock::millisecs(timeout));
	setOption(SOL_SOCKET, SO_SNDTIMEO, ms);
#else
    struct timeval t = Clock::toTimeVal(timeout);
	setOption(SOL_SOCKET, SO_SNDTIMEO, t);
#endif
}

Clock::duration Socket::getSendTimeout()
{
#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
	int32_t ms = getOption<int32_t>(SOL_SOCKET, SO_SNDTIMEO);
	return Clock::duration(Clock::millisec(ms));
#else
    struct timeval tv = getOption<timeval>(SOL_SOCKET, SO_SNDTIMEO);
	return Clock::fromTimeVal(tv);
#endif
}

void Socket::setReceiveTimeout(Clock::tick_type timeout)
{
#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
    // ms winsock timeout is in milliseconds
	int32_t ms = static_cast<int32_t>(Clock::millisecs(timeout));
	setOption(SOL_SOCKET, SO_RCVTIMEO, ms);
#else
    struct timeval t = Clock::toTimeVal(timeout);
	setOption(SOL_SOCKET, SO_RCVTIMEO, t);
#endif
}

Clock::duration Socket::getReceiveTimeout()
{
#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
	int32_t ms = getOption<int32_t>(SOL_SOCKET, SO_RCVTIMEO);
	return Clock::duration(Clock::millisec(ms));
#else
    struct timeval tv = getOption<timeval>(SOL_SOCKET, SO_RCVTIMEO);
	return Clock::fromTimeVal(tv);
#endif
}

SocketAddress Socket::address()
{
	if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "invalid address", false);
    }

	struct sockaddr_storage addr_storage;
    socklen_t addr_sz = sizeof(addr_storage);
    if (::getsockname(fd_, (struct sockaddr*)&addr_storage, &addr_sz) < 0)
    {
        SYS_ERR_THROW(NetException);
    }
    return SocketAddress(addr_storage);
}

SocketAddress Socket::peerAddress()
{
	if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "invalid address", false);
    }

  	struct sockaddr_storage remote_addr;
    socklen_t addr_sz = sizeof(remote_addr);  
    if (::getpeername(fd_, (struct sockaddr*)&remote_addr, &addr_sz) <0 )
    {
        SYS_ERR_THROW(NetException);
    }
    return SocketAddress(remote_addr);
}


//-----------------------------------------------------------------------------------------
// socket operations
//-----------------------------------------------------------------------------------------
Socket* Socket::accept()
{
	if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "accept from an invalid socket", false);
    }

	sockaddr_storage client_addr;
    socklen_t addr_sz = sizeof(client_addr); 
	SocketId accept_fd;
	do
	{
		accept_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_sz);
	}
	while (accept_fd == INVALID_SOCK_ID && errno == EINTR);

	if (accept_fd == INVALID_SOCK_ID)
	{
        SYS_ERR_THROW(NetException, "accept failed");
    }

	return new Socket(accept_fd);
}

int Socket::connect(const SocketAddress& addr)
{
	ensureOpen(addr.af(), SOCK_STREAM);
    scopedEnsure(*this, SocketFlag::NonBlock, true);

	int rc = ::connect(fd_, static_cast<const sockaddr*>(addr.addr()),
                       addr.addrLength());

    return rc;
}

void Socket::bind(const SocketAddress& addr)
{
	ensureOpen(addr.af(), SOCK_STREAM);

	int rc = ::bind(fd_, static_cast<const sockaddr*>(addr.addr()), addr.addrLength());

	if (rc != 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Bind to " << addr.toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
}

void Socket::listen(int backlog)
{
	if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException,"listen on invalid socket", false);
    }

	int rc = ::listen(fd_, backlog);
	if (rc != 0)
    {
        SYS_ERR_THROW(NetException, "listen failed");
    }
}

void Socket::shutdownSend()
{
	if (fd_ != INVALID_SOCK_ID)
    {
        if (::shutdown(fd_, SHUT_WR) != 0)
        {
            SYS_ERR_THROW(NetException, "Cannot shutdown send");
        }
    }
}

void Socket::shutdownRecv()
{
	if (fd_ != INVALID_SOCK_ID)
    {
        if (::shutdown(fd_, SHUT_RD) != 0)
        {
            SYS_ERR_THROW(NetException, "Cannot shutdown receive");
        }
    }
}

void Socket::shutdown()
{
	if (fd_ != INVALID_SOCK_ID)
    {
        if (::shutdown(fd_, SHUT_RDWR) != 0)
        {
            SYS_ERR_THROW(NetException, "Cannot shutdown");
        }
    }
}

ssize_t Socket::send(const void* buffer, int length, int flags)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "send on invalid socket", false);
    }

    ssize_t res = ::send(fd_, reinterpret_cast<const char*>(buffer), length, flags);
    
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Send from " << address().toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::send(const struct iovec *iov, int iovcnt, int flags)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "send on invalid socket", false);
    }

    struct msghdr msgHdr;
    msgHdr.msg_name = nullptr;
    msgHdr.msg_namelen = 0;
    msgHdr.msg_iov = const_cast<iovec*>(iov);
    msgHdr.msg_iovlen = iovcnt;
    msgHdr.msg_control = 0;
    msgHdr.msg_controllen = 0;
    msgHdr.msg_flags = flags;
    ssize_t res = sendmsg(fd_, &msgHdr, flags);
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Send from " << address().toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::sendTo(const void* buffer, int length, const SocketAddress& addr, int flags)
{
    ensureOpen(addr.af(), SOCK_DGRAM);
	ssize_t res = ::sendto(fd_, reinterpret_cast<const char*>(buffer), length,
                    flags, addr.addr(), addr.addrLength());
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Send to " << addr.toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::sendTo(const struct iovec *iov, int iovcnt, const SocketAddress& addr, int flags)
{
    ensureOpen(addr.af(), SOCK_DGRAM);
    struct msghdr msgHdr;
    msgHdr.msg_name = const_cast<sockaddr*>(addr.addr());
    msgHdr.msg_namelen = addr.addrLength();
    msgHdr.msg_iov = const_cast<iovec*>(iov);
    msgHdr.msg_iovlen = iovcnt;
    msgHdr.msg_control = 0;
    msgHdr.msg_controllen = 0;
    msgHdr.msg_flags = flags;
    ssize_t res = sendmsg(fd_, &msgHdr, flags);
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Send to " << addr.toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::receive(void* buffer, int length, int flags)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "receive on invalid socket", false);
    }

    ssize_t res = ::recv(fd_, reinterpret_cast<char*>(buffer), length, flags);
    
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Receve at " << address().toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::receive(struct iovec *iov, int iovcnt, int flags)
{
    if (fd_ == INVALID_SOCK_ID)
    {
        SYS_ERR_THROW(NetException, "send on invalid socket", false);
    }

    struct msghdr msgHdr;
    msgHdr.msg_name = nullptr;
    msgHdr.msg_namelen = 0;
    msgHdr.msg_iov = iov;
    msgHdr.msg_iovlen = iovcnt;
    msgHdr.msg_control = 0;
    msgHdr.msg_controllen = 0;
    msgHdr.msg_flags = flags;
    ssize_t res = recvmsg(fd_, &msgHdr, flags);
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Receve at " << address().toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::recvFrom(void* buffer, int length, const SocketAddress& addr, int flags)
{
    ensureOpen(addr.af(), SOCK_DGRAM);
    socklen_t addrlen = addr.addrLength();
	ssize_t res = ::recvfrom(fd_, reinterpret_cast<char*>(buffer), length, flags,
                    const_cast<sockaddr*>(addr.addr()), &addrlen);

    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Receive from " << addr.toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::recvFrom(struct iovec *iov, int iovcnt, const SocketAddress& addr, int flags)
{
    ensureOpen(addr.af(), SOCK_DGRAM);
    struct msghdr msgHdr;
    msgHdr.msg_name = const_cast<sockaddr*>(addr.addr());
    msgHdr.msg_namelen = addr.addrLength();
    msgHdr.msg_iov = const_cast<iovec*>(iov);
    msgHdr.msg_iovlen = iovcnt;
    msgHdr.msg_control = 0;
    msgHdr.msg_controllen = 0;
    msgHdr.msg_flags = flags;
    ssize_t res = recvmsg(fd_, &msgHdr, flags);
    if (res < 0)
    {
        StrPrinter<128> err_msg;
        err_msg << "Receive from " << addr.toString() << " failed";
        SYS_ERR_THROW(NetException, err_msg.c_str());
    }
    return res;
}

ssize_t Socket::available()
{
	int data_available = 0;
    int rc = ::ioctl(fd_, FIONREAD, &data_available);
    if (rc != 0)
    {
        SYS_ERR_THROW(NetException);
    }
	return data_available;
}

#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
int SockAPIInit()
{
    static bool initialized = false;
    static int  err = 0;
    if (!initialized)
    {
        // Initialize Windows Socket API with given VERSION.
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0)
        {
            // We could not find a usable Winsock DLL
            StrPrinter<128> err_msg;
            err_msg << "WSAStartup failed with error: " << err ;
            SYS_ERR_THROW(NetException, err_msg.c_str(), false);
            return -1;
        }
        initialized = true;
    }
    return err;
}

void SockAPICleanup() { WSACleanup(); }
#endif

} // namespace alt
