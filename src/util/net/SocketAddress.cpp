#include "SocketAddress.h"
#include "DNS.h"

#include <util/system/SysError.h>       // for SYS_ERROR_LOG
#include <util/string/StrPrint.h>       // for StrPrinter
#include <util/string/StrScan.h>        // for StrParser
#include <util/types/TemplateHelper.h>  // for hash_combine

#include <net/if.h>                     // for if_nametoindex

namespace alt {

SocketAddress::SocketAddress()
{
    storage_.ss_family = 0;
}

SocketAddress::SocketAddress(
    const IPAddress& addr,
    PortId port
#if ALT_IPV6_AVAILABLE
    , uint32_t flowinfo
    , uint32_t scope
#endif
    )
{
#if ALT_IPV6_AVAILABLE
    if (addr.family()==IPFamily::IPv6)
    {
        memcpy(&storage_, addr.addr(), sizeof(sockaddr_in6));
        return;
    }
#endif
    if (addr.family()==IPFamily::IPv4)
    {
        memcpy(&storage_, addr.addr(), sizeof(sockaddr_in));
        return;
    }
    storage_.ss_family = 0;
}

IPFamily SocketAddress::family() const
{
    return storage_.ss_family==AF_INET6 ? IPFamily::IPv6 :
           storage_.ss_family==AF_INET ? IPFamily::IPv4 : IPFamily::Unset;
}

int SocketAddress::af() const
{
    return storage_.ss_family;
}

const sockaddr* SocketAddress::addr() const
{
    return reinterpret_cast<const sockaddr*>(&storage_);
}

socklen_t SocketAddress::addrLength() const
{
    return storage_.ss_family==AF_INET6
            ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
}

PortId SocketAddress::port() const
{
    return storage_.ss_family==AF_INET6
        ? betoh(reinterpret_cast<const sockaddr_in6*>(&storage_)->sin6_port)
        : betoh(reinterpret_cast<const sockaddr_in*>(&storage_)->sin_port); 
}

#if ALT_IPV6_AVAILABLE

uint32_t SocketAddress::flowInfo() const
{
    return storage_.ss_family==AF_INET6
        ? betoh(reinterpret_cast<const sockaddr_in6*>(&storage_)->sin6_flowinfo)
        : 0; 
}

uint32_t SocketAddress::scope() const
{
    return storage_.ss_family==AF_INET6
        ? betoh(reinterpret_cast<const sockaddr_in6*>(&storage_)->sin6_scope_id)
        : 0; 
}

#endif

SocketAddress::SocketAddress(const struct sockaddr_storage& ip_addr)
{
    fromRawFormat(ip_addr);
}

void SocketAddress::fromRawFormat(const struct sockaddr_storage& ip_addr) noexcept(false)
{
#if ALT_IPV6_AVAILABLE
    if (ip_addr.ss_family == AF_INET6)
    {
        memcpy(&storage_, &ip_addr, sizeof(sockaddr_in6));
        return;
    }
#endif
    if (ip_addr.ss_family == AF_INET)
    {
        memcpy(&storage_, &ip_addr, sizeof(sockaddr_in));
    }
    else
    {
        SYS_ERR_THROW(NetException, "IPv6 not supported", false);
    }
}

void SocketAddress::toRawForamt(struct sockaddr_storage& ip_addr)
{
    auto f = family();
    if (f == IPFamily::Unset)
    {
        SYS_ERR_THROW(NetException, "Access uninitialized IP address", false);
    }
#if ALT_IPV6_AVAILABLE
    if (f == IPFamily::IPv6)
    {
        memcpy(&ip_addr, &storage_, sizeof(sockaddr_in6));
        return;
    }
#endif
    if (f == AF_INET)
    {
        memcpy(&ip_addr, &storage_, sizeof(sockaddr_in));
    }
    else
    {
        SYS_ERR_THROW(NetException, "IPv6 not supported", false);
    }
}

const void* SocketAddress::ipAddr(socklen_t& addr_length) const
{
    if (storage_.ss_family==AF_INET6)
    {
        addr_length = sizeof(in6_addr);
        return &(reinterpret_cast<const sockaddr_in6*>(&storage_)->sin6_addr);
    }
    if (storage_.ss_family==AF_INET)
    {
        addr_length = sizeof(in_addr);
        return &(reinterpret_cast<const sockaddr_in*>(&storage_)->sin_addr);
    }
    addr_length = 0;
    return nullptr;
}

IPAddress SocketAddress::ipAddr() const
{
    socklen_t addr_length;
    const void* ip_addr = ipAddr(addr_length);
    return IPAddress (ip_addr, addr_length);
}

std::string SocketAddress::toString() const
{
    auto f = family();
    if (f == IPFamily::Unset)
    {
        return std::string("INVALID");
    }

    std::string ip_str = ipAddr().toString();

#if ALT_IPV6_AVAILABLE
    if (f == IPFamily::IPv6)
    {
        StrPrinter<INET6_ADDRSTRLEN+16> spr;
        spr << '(' << ip_str << ", " << port() << ", "
            << flowInfo() << ", " << scope() << ')';
        return std::string(spr.c_str());
    }
#endif
    StrPrinter<16> spr;
    spr << ':' << port();
    return ip_str + spr.c_str();
}

SocketAddress SocketAddress::fromString(const char* addr, size_t length, PortId default_port)
{
    if (!length)
    {
        length = addr ? strlen(addr) : 0;
    }
    if (!length)
    {
        return SocketAddress();
    }

    PortId    port = default_port;
    IPAddress ipaddr;

#if ALT_IPV6_AVAILABLE   
    uint32_t scope = 0;
    uint32_t flowinfo =0;
#endif

    StrParser strscan(addr, length);
    char ch = strscan.skipWhiteSpace();
    if (ch=='[')
    {
        // IPv6 addresses is enclosed in brackets []
        strscan.advance();    // skip [
        // in the format of [<ip_address>]:port
        std::string ip_addr_str;
        // scan to char '%' or ']' to get a substring from current position
        ch = strscan.getSubstring("%]", ip_addr_str);
        ip_addr_str = strTrim(ip_addr_str);
        ipaddr = IPAddress(ip_addr_str);
        if (ch=='%')
        {
            // scope is specified
#if ALT_IPV6_AVAILABLE
            strscan.advance();   // skip %
            std::string scope_str;
            ch = strscan.getSubstring(']', ip_addr_str, false);
            scope_str = strTrim(scope_str);
            scope = if_nametoindex(scope_str.c_str());
            if (scope == 0)
            {
                SYS_ERR_THROW(NetException, "Invalid scopt name", false);
            }
#else
            SYS_ERR_THROW(NetException, "Invalid IPv4 address", false);
#endif         
        }
        if (ch!=']')
        {
            SYS_ERR_THROW(NetException, "Invalid IP address, nissing ']'", false);
        }
        ch = strscan.nextNonWhiteSpace();
        if (ch==':')
        {
            strscan.nextNonWhiteSpace();
            strscan >> port;
        }
    }
    else if (ch=='(')
    {
        // in python-like format: (<ip_address>, <port>opt, <flowinfo>opt, <scope>opt)
        // Note: this is not the standard host name in URL address
        strscan.advance();
        std::vector<std::string> strings;
        strscan.setTerminator(')');
        strscan.setSkipLeadingSp(true);
        strscan.setSkipTrailingSp(true);
        strscan >> strings;
        strscan.setTerminator('\0');
#if ALT_IPV6_AVAILABLE
        if (strings.size()==4)
        {
            scope = if_nametoindex(strings[3].c_str());
            if (scope == 0)
            {
                SYS_ERR_THROW(NetException, "Invalid scope name", false);
            }
        }
        if (strings.size()>=3)
        {
            StrParser flowinfo_parser(strings[2]);
            flowinfo_parser >> flowinfo;
        }
#endif
        if (strings.size()>=2)
        {
            StrParser port_parser(strings[1]);
            port_parser >> port;
        }
        if (strings.size()>=1)
        {
           ipaddr = IPAddress(strings[1]);
        }       
    }
    else
    {
        std::string ip_str;
        std::string port_str;
        const char* cp = strchr(addr, ':');
        if (cp != nullptr)
        {
            // addr contains port
            ip_str.append(addr, size_t(cp-addr));
            port_str.append(cp+1);
        }
        else
        {
            ip_str.append(addr);
        }
        bool is_valid_ipv4_addr = false;
        try
        {
            ipaddr = IPAddress(ip_str);
            is_valid_ipv4_addr = true;
        }
        catch (...)
        {
        }

        if (is_valid_ipv4_addr)
        {
            if (!port_str.empty())
            {
                StrParser port_parser(port_str);
                port_parser >> port;
            }
            return SocketAddress(ipaddr, port);
        }

        // a network hostname, whose network addresses needs to be resolved
        const DNS::HostInfo & host_info = DNS::instance().getHostInfo(addr);
        const sockaddr_storage* sock_addr = host_info.getSocketAddress(IPFamily::IPv4);
#if ALT_IPV6_AVAILABLE
        if (!sock_addr)
        {
            sock_addr = host_info.getSocketAddress(IPFamily::IPv6);
        }
#endif
        if (sock_addr)
        {
            return SocketAddress(*sock_addr);
        }

        StrPrinter<128> err;
        err <<  "Unresolved host name or invalid ip address: '" << addr <<'/';
        SYS_ERR_THROW(NetException, err.c_str(), false);
    }

#if ALT_IPV6_AVAILABLE   
    return SocketAddress(ipaddr, port, flowinfo, scope);
#else
    return SocketAddress(ipaddr, port);
#endif
}

size_t SocketAddress::hash() const
{
    std::size_t seed=0;
    size_t addr_hash =ipAddr().hash();
#if ALT_IPV6_AVAILABLE
    if (family()==IPFamily::IPv6)
    {
        hash_combine(seed, addr_hash, port(), flowInfo(), scope());
        return seed;
    }
#endif
    hash_combine(seed, addr_hash, port());
    return seed;
}

}