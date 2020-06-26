
#include "IPAddress.h"

#include <util/string/StrUtils.h>           // For strTrim
#include <util/string/StrScan.h>            // For StrParser
#include <util/types/TemplateHelper.h>      // for hash_combine and make_arrary

#include <string.h>                         // For strlen
#include <arpa/inet.h>                      // For inet_ntop

namespace alt {

//-----------------------------------------------------------------------------
// IPv4ImplDefault
//-----------------------------------------------------------------------------
class IPAddress::IPv4ImplDefault: public IPv4Impl
{
public:
	IPv4ImplDefault()
    {
        std::memset(&addr_, 0, sizeof(addr_));
    }

	IPv4ImplDefault(const void* addr)
    {
        std::memcpy(&addr_, addr, sizeof(addr_));
    }

    IPv4ImplDefault(struct in_addr&& addr)
    {
        std::memcpy(&addr_, &addr, sizeof(addr_));
    }

    IPv4ImplDefault(const struct in_addr& addr)
    {
        std::memcpy(&addr_, &addr, sizeof(addr_));
    }

	IPv4ImplDefault(const IPv4ImplDefault& other): IPv4ImplDefault(&other.addr_) {}

    IPv4ImplDefault(unsigned mask_bits)
    {
	    addr_.s_addr =
            htobe(uint32_t((mask_bits == 32) ? 0xFFFFFFFFU : ~(0xFFFFFFFFU >> mask_bits)));
    }

	int af() const override { return AF_INET; }
    socklen_t addrLength() const override { return sizeof(addr_); }
	const void* addr() const override { return &addr_; }

	IPv4ImplDefault& operator = (const IPv4ImplDefault& other)
    {
        if (&other!=this) std::memcpy(&addr_, &other.addr_, addrLength());
        return *this;
    }

	std::string toString() const
    {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_, ipstr, sizeof(ipstr));
        return std::string(ipstr);
    }

	bool isWildcard() const override { return addr_.s_addr == INADDR_ANY; }

	bool isBroadcast() const override { return addr_.s_addr == INADDR_NONE; }

	bool isLoopback() const override
    {
        // range from 127.0.0.1 to 127.255.255.255
	    return (betoh(addr_.s_addr) & 0xFF000000U) == 0x7F000000U; 
    }

	bool isMulticast() const override
    {
        // range from 224.0.0.1/24 to 239.0.0.0/24
        return (betoh(addr_.s_addr) & 0xF0000000U) == 0xE0000000U;
    }

	bool isLinkLocal() const override
    {
        // range from 169.254.0.0/16 to 169.254.255.255/16
        return (betoh(addr_.s_addr) & 0xFFFF0000U) == 0xA9FE0000U;
    }

	bool isSiteLocal() const override
    {
        uint32_t addr = betoh(addr_.s_addr);
	    return (addr & 0xFF000000U) == 0x0A000000U ||     // 10.0.0.0/24 ~ 10.255.255.255/24
            (addr & 0xFFFF0000U) == 0xC0A80000U ||        // 192.68.0.0/16 ~ 192.68.255.255/16
            (addr >= 0xAC100000U && addr <= 0xAC1FFFFFU); // 172.16.0.0 ~ 172.31.255.255
    }

	bool isIPv4Compatible() const override { return true; }
	bool isIPv4Mapped() const override { return true; }

	IPv4ImplDefault operator & (const IPv4ImplDefault& other) const
    {
        return IPv4ImplDefault (in_addr{addr_.s_addr & other.addr_.s_addr});
    }

	IPv4ImplDefault operator | (const IPv4ImplDefault& other) const
    {
        return IPv4ImplDefault (in_addr{addr_.s_addr | other.addr_.s_addr});
    }
    
	IPv4ImplDefault operator ^ (const IPv4ImplDefault& other) const
    {
        return IPv4ImplDefault (in_addr{addr_.s_addr ^ other.addr_.s_addr});
    }
    
#if 0
    // This is not required
	IPv4ImplDefault operator ~ () const
    {
        return IPv4ImplDefault (in_addr{addr_.s_addr ^ 0xFFFFFFFFU});
    }
#endif
    
    bool operator == (const IPv4ImplDefault& other) const
    {
        return std::memcmp(&addr_, &other.addr_, sizeof(addr_)==0);
    }

	bool operator != (const IPv4ImplDefault& other) const
    {
        return !(*this == other);
    }
};


#if ALT_IPV6_AVAILABLE
//-----------------------------------------------------------------------------
// IPv6ImplDefault
//-----------------------------------------------------------------------------
class IPAddress::IPv6ImplDefault: public IPv6Impl
{
public:
	IPv6ImplDefault()
    {
        std::memset(&addr_, 0, sizeof(addr_));
    }

	IPv6ImplDefault(const void* addr)
    {
        std::memcpy(&addr_, addr, sizeof(addr_));
    }

    IPv6ImplDefault(struct in6_addr&& addr)
    {
        std::memcpy(&addr_, &addr, sizeof(addr_));
    }

    IPv6ImplDefault(const struct in6_addr& addr)
    {
        std::memcpy(&addr_, &addr, sizeof(addr_));
    }

	IPv6ImplDefault(const IPv6ImplDefault& other)
        : IPv6ImplDefault(&other.addr_) {}

	IPv6ImplDefault(unsigned mask_bits)
    {
        uint64_t* u64p = reinterpret_cast<uint64_t*>(&addr_);
        if (mask_bits < 64)
        {
            u64p[0] = htobe(~(0xFFFFFFFFFFFFFFFFUL >> mask_bits));
            u64p[1] = 0UL;
        }
        else
        {
            u64p[0] = 0xFFFFFFFFFFFFFFFFUL;
            u64p[1] = mask_bits <128 ? htobe(~(0xFFFFFFFFFFFFFFFFUL >> (mask_bits-64)))
                                  : 0xFFFFFFFFFFFFFFFFUL;
            
        }
    }

	int af() const override { return AF_INET6; }
    socklen_t addrLength() const override { return sizeof(addr_); }
	const void* addr() const override { return &addr_; }

	IPv6ImplDefault& operator = (const IPv6ImplDefault& other)
    {
        if (&other!=this) {
            std::memcpy(&addr_, &other.addr_, addrLength());
        }
        return *this;
    }

	std::string toString() const
    {
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr_, ipstr, sizeof(ipstr));
        return std::string(ipstr);
    }

	bool isWildcard() const override
    {
        return *reinterpret_cast<const uint64_t*>(&addr_)==0L &&
               *(reinterpret_cast<const uint64_t*>(&addr_)+1)==0L;
    }

	bool isBroadcast() const override
    {
        // no distinct concept of a broadcast address in IPv6
        return false;
    }

    bool isLoopback() const override
    {
        return *reinterpret_cast<const uint64_t*>(&addr_)==0L &&
               betoh(*(reinterpret_cast<const uint64_t*>(&addr_)+1))==1L;
    }

    bool isMulticast() const override
    {
        return betoh(*reinterpret_cast<const uint16_t*>(&addr_)) & 0xFFE0==0xFF00;
    }

	bool isLinkLocal() const override
    {
        return betoh(*reinterpret_cast<const uint16_t*>(&addr_)) & 0xFFE0==0xFE80;
    }

	bool isSiteLocal() const override
    {
        return betoh(*reinterpret_cast<const uint16_t*>(&addr_)) & 0xFFE0==0xFEC0 ||
               betoh(*reinterpret_cast<const uint16_t*>(&addr_)) & 0xFF00==0xFC00;
    }

	bool isIPv4Compatible() const override
    {
        return *reinterpret_cast<const uint64_t*>(&addr_)==0L &&
               *(reinterpret_cast<const uint64_t*>(&addr_)+1) & 0xFFFFFFFFUL==0UL;
    }

	bool isIPv4Mapped() const override
    {
        return *reinterpret_cast<const uint64_t*>(&addr_)==0L &&
               *(reinterpret_cast<const uint64_t*>(&addr_)+1) & 0xFFFFFFFFUL==0xFFFF0000UL;
    }

	IPv6ImplDefault operator & (const IPv6ImplDefault& addr) const
    {
        IPv6ImplDefault res(this);
        uint64_t* u64p = reinterpret_cast<uint64_t*>(&res.addr_);
        u64p[0] &= u64p[0];
        u64p[1] &= u64p[1];
        return res;
    }

	IPv6ImplDefault operator | (const IPv6ImplDefault& addr) const
    {
        IPv6ImplDefault res(this);
        uint64_t* u64p = reinterpret_cast<uint64_t*>(&res.addr_);
        u64p[0] |= u64p[0];
        u64p[1] |= u64p[1];
        return res;
    }

	IPv6ImplDefault operator ^ (const IPv6ImplDefault& addr) const
    {
        IPv6ImplDefault res(this);
        uint64_t* u64p = reinterpret_cast<uint64_t*>(&res.addr_);
        u64p[0] ^= u64p[0];
        u64p[1] ^= u64p[1];
        return res;
    }

#if 0
    // This is not required
	IPv6ImplDefault operator ~ () const
    {
        IPv6ImplDefault res(this);
        uint64_t* u64p = reinterpret_cast<uint64_t*>(&res.addr_);
        u64p[0] ^= 0xFFFFFFFFFFFFFFFFUL;
        u64p[1] ^= 0xFFFFFFFFFFFFFFFFUL;
        return res;
    }
#endif

	bool operator == (const IPv6ImplDefault& other) const
    {
        return std::memcmp(&addr_, &other.addr_, sizeof(addr_)==0);
    }

	bool operator != (const IPv6ImplDefault& other) const
    {
        return !(*this == other);
    }
};
#endif

//-----------------------------------------------------------------------------
// IPAddress
//-----------------------------------------------------------------------------
IPAddress::IPAddress(IPFamily family)
{
#if ALT_IPV6_AVAILABLE
    if (family == IPFamily::IPv6)
    {
        new (storage()) IPv6ImplDefault();
        return;
    }
#endif
    new (storage()) IPv4ImplDefault();
}

IPAddress::IPAddress(const IPAddress& other)
{
#if ALT_IPV6_AVAILABLE
    if (other.family() == IPFamily::IPv6)
    {
        auto other6 = reinterpret_cast<const IPv6ImplDefault&>(other);
        new (storage()) IPv6ImplDefault(other6.addr());
        return;
    }
#endif
    new (storage()) IPv4ImplDefault();   
}

IPAddress::IPAddress(const char* addr_str, size_t length, IPFamily family) 
{
    fromString(addr_str, length, family);
}

IPAddress::IPAddress(unsigned mask_bits, IPFamily family)
{
#if ALT_IPV6_AVAILABLE
    if (family == IPFamily::IPv6)
    {
        new (storage()) IPv6ImplDefault(mask_bits);
        return;
    }
#endif
    new (storage()) IPv4ImplDefault(mask_bits);      
}

IPAddress::IPAddress(const void* addr, socklen_t length)
{
	if (length == sizeof(struct in_addr))
    {
		new (storage()) IPv4ImplDefault(addr);
        return;
    }
#if ALT_IPV6_AVAILABLE
	if (length == sizeof(struct in6_addr))
    {
		new (storage()) IPv6ImplDefault(addr);
        return;
    }
#endif
	throw ("Invalid address length to construct IPAddress");
}

IPAddress IPAddress::binaryOP (const IPAddress& other, IPBinaryOP op) const
{
    auto f = family();
    if (f != other.family())
    {
        throw ("Binary operation on different IP families");
    }

#if ALT_IPV6_AVAILABLE
    if (f == IPFamily::IPv6)
    {
        static auto ops = make_array
         ( [](const IPv6ImplDefault& p1, const IPv6ImplDefault& p2)->IPv6ImplDefault { return p1 & p2; },
           [](const IPv6ImplDefault& p1, const IPv6ImplDefault& p2)->IPv6ImplDefault { return p1 | p2; },
           [](const IPv6ImplDefault& p1, const IPv6ImplDefault& p2)->IPv6ImplDefault { return p1 ^ p2; }
         );
        const IPv6ImplDefault p1(implPtr()->addr());
        const IPv6ImplDefault p2(other.implPtr()->addr());
        const IPv6ImplDefault res = ops[op](p1, p2);
        return IPAddress(res.addr(), res.addrLength());
    }
#endif

    static auto ops = make_array
        ( [](const IPv4ImplDefault& p1, const IPv4ImplDefault& p2)->IPv4ImplDefault { return p1 & p2; },
          [](const IPv4ImplDefault& p1, const IPv4ImplDefault& p2)->IPv4ImplDefault { return p1 | p2; },
          [](const IPv4ImplDefault& p1, const IPv4ImplDefault& p2)->IPv4ImplDefault { return p1 ^ p2; }
        );
    const IPv4ImplDefault p1(implPtr()->addr());
    const IPv4ImplDefault p2(other.implPtr()->addr());
    const IPv4ImplDefault res = ops[op](p1, p2);
    return IPAddress(res.addr(), res.addrLength());
}

IPAddress IPAddress::operator & (const IPAddress& other) const
{
    return binaryOP (other, BitAnd);
}

IPAddress IPAddress::operator | (const IPAddress& other) const
{
    return binaryOP (other, BitOr);
}

IPAddress IPAddress::operator ^ (const IPAddress& other) const
{
    return binaryOP (other, BitXor);
}

IPAddress IPAddress::operator ~ () const
{
#if ALT_IPV6_AVAILABLE
    if (family() == IPFamily::IPv6)
    {
        return binaryOP (IPAddress(unsigned(128), IPFamily::IPv6), BitXor);
    }
#endif
    return binaryOP (IPAddress(unsigned(32), IPFamily::IPv4), BitXor);
}

size_t IPAddress::hash() const
{
#if ALT_IPV6_AVAILABLE
    if (family()==IPFamily::IPv6)
    {
        std::size_t seed=0;
        const uint64_t* u64p = reinterpret_cast<const uint64_t*>(implPtr()->addr());
        hash_combine(seed, u64p[0], u64p[1]);
        return seed;
    }
#endif
    return reinterpret_cast<const struct in_addr*>(implPtr()->addr())->s_addr;
}
    
void IPAddress::fromString(const char* addr, size_t length, IPFamily family)
{
    if (length==0)
    {
        length = addr ? strlen(addr) : 0;
    }

    // Empty address
    if (length==0)
    {
#if ALT_IPV6_AVAILABLE
        if (family == IPFamily::IPv6)
        {
            new (storage()) IPv6ImplDefault();
            return;
        }
#endif
        new (storage()) IPv4ImplDefault();
        return;
    }

    // skip leading and trailing white spaces
    const char* ip_addr_begin;
    const char* ip_addr_end;
    strTrim(addr, length, ip_addr_begin, ip_addr_end);

    // Empty IPv6 address
    if (strncmp(ip_addr_begin, "::", 2)==0 && length==2)
    {
        if (family == IPFamily::IPv4)
        {
            throw ("\"::\" is not a valid IPv4 address");
        }
#if ALT_IPV6_AVAILABLE
        new (storage()) IPv6ImplDefault();
#else
        throw ("IPv6 address \"::\" is not available");
#endif
    }

    // localhost address
    if (strcmp(ip_addr_begin,"0.0.0.0")==0)
    {
        new (storage()) IPv4ImplDefault();
        return;  
    }

    // localhost address
    if (strcmp(ip_addr_begin,"localhost")==0 ||
        strcmp(ip_addr_begin,"LOCALHOST")==0
    )
    {
#if ALT_IPV6_AVAILABLE
        if (family == IPFamily::IPv6)
        {
            uint16_t addr_vec[8] { 0, 0, 0, 0, 0, 0, 0, 1 };
            new (storage()) IPv6ImplDefault(addr_vec);
            return;
        }
#endif
        uint8_t addr_vec[4] { 127, 0, 0, 1 };
        new (storage()) IPv4ImplDefault(addr_vec);
        return;
    }

    // check if it is ipv4
    StrParser scan(ip_addr_begin, ip_addr_end + 1);
    uint64_t first_sec = scan.getUnsigned();
    bool is_v4 {false};
    if (*ip_addr_begin=='0' && scan.pos()==10 &&
        (*(ip_addr_begin+1)=='x' || *(ip_addr_begin+1)=='X'))
    {
        // in the format of 0xc0a8010a
        is_v4 = true;
    }
    else if (first_sec>=0 && first_sec<=255 && *scan.nextScanStr()=='.')
    {
        // in the format starting with 192. or 0xC0. etc
        is_v4 = true;
    }

    if (!is_v4 && family == IPFamily::IPv4)
    {
        throw ("not a valid IPv4 address");
    }

#if ALT_IPV6_AVAILABLE
    if (!is_v4)
    {
        std::string  addr_str(ip_addr_begin, size_t(ip_addr_end-ip_addr_begin+1));
        struct in6_addr  ipv6_addr;
        if (inet_pton(AF_INET6, addr_str.c_str(), &ipv6_addr) == 1)
        {
            new (storage()) IPv6ImplDefault(&ipv6_addr);
        }
        else
        {
            throw ("Invalid IP address");
        }
    }
    else
#endif
    {
        std::string  addr_str(ip_addr_begin, size_t(ip_addr_end-ip_addr_begin+1));
        struct in_addr  ipv4_addr;
        // inet_aton allows more general numbers-and-dots notation for IPv4 addresses
        // but not support IPv6
        if (inet_aton(addr_str.c_str(), &ipv4_addr) == 1)
        {
            new (storage()) IPv4ImplDefault(&ipv4_addr);
        }
        else
        {
            throw ("Invalid IP address");
        }
    }
}

}
