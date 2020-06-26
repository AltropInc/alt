#pragma once

#include "IPAddress.h"

#include <sys/socket.h>   // for sockaddr_storage

namespace alt {

using PortId = uint16_t;
constexpr PortId WILDCARD_PORT_ID = 0;

class ALT_UTIL_PUBLIC SocketAddress
{
  public:
    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
 
    /// \brief construct an empty uninitialized address
    /// (family()==IPFamily::Unset)
    SocketAddress();
    
    /// \brief construct a socket address by a given io address and port
    SocketAddress(const IPAddress& addr,
                  PortId port
#if ALT_IPV6_AVAILABLE
                  , uint32_t flowinfo_ =0
                  , uint32_t scope_ =0
#endif
    );

    /// \brief construct a socket address from the native one
    SocketAddress(const struct sockaddr_storage& ip_addr) noexcept(false);

    void fromRawFormat(const struct sockaddr_storage& ip_addr) noexcept(false);

    void toRawForamt(struct sockaddr_storage& ip_addr) noexcept(false);

    static SocketAddress fromString(
        const char* addr,
        size_t length=0,
        PortId default_port=WILDCARD_PORT_ID);

    template <typename StrT>
    static SocketAddress fromString(const StrT& addr, PortId default_port=WILDCARD_PORT_ID)
    { return fromString(addr.c_str(), addr.length(), default_port); }

    std::string toString() const;

    size_t hash() const;

    IPFamily family() const;

    int af() const;

    const void* ipAddr(socklen_t& addr_length) const;

    IPAddress ipAddr() const;

    const sockaddr* addr() const;

    socklen_t addrLength() const;

    PortId port() const;

#if ALT_IPV6_AVAILABLE

    uint32_t flowInfo() const;
    
    uint32_t scope() const;

#endif

  private:
    struct sockaddr_storage  storage_;
    /*
    IPAddress        addr_;
    PortId           port_ {WILDCARD_PORT_ID};
#if ALT_IPV6_AVAILABLE
    uint32_t         flowinfo_ {0};
    uint32_t         scope_ {0};
#endif
    */
};

}  // namespace alt

namespace std {
template <>
struct hash<alt::SocketAddress>
{
    size_t operator()(const alt::SocketAddress& key) const noexcept
    {
        return key.hash();
    }
};
} // namespace std



#if 0
struct sockaddr {
   unsigned short    sa_family;    // address family, AF_xxx
   char              sa_data[14];  // 14 bytes of protocol address
};


struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};


struct sockaddr_in6 {
    u_int16_t       sin6_family;   // address family, AF_INET6
    u_int16_t       sin6_port;     // port number, Network Byte Order
    u_int32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    u_int32_t       sin6_scope_id; // Scope ID
};


  struct sockaddr_storage {
    short ss_family;
    char __ss_pad1[_SS_PAD1SIZE];

    __MINGW_EXTENSION __int64 __ss_align;
    char __ss_pad2[_SS_PAD2SIZE];
  };

struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];
};
#endif