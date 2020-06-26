
#pragma once

#include <util/system/Platform.h>
#include <util/types/Enum.h>
#include <util/Defs.h>

#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
    #include <Winsock2.h>
    #include <Ws2tcpip.h>
    #include <Ws2def.h>
    #include <Windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <time.h>
    #include <unistd.h>
#endif
#include <string>       // for std::string
#include <string.h>     // for memcpy

#define in_addr6

#ifdef in_addr6
#define ALT_IPV6_AVAILABLE       1
#else
#define ALT_IPV6_AVAILABLE       0
#endif

namespace alt {

#ifndef socklen_t
    using socklen_t = int;
#endif

ENUM(IPFamily, uint8_t, Unset, IPv4, IPv6);

class ALT_UTIL_PUBLIC IPAddress
{
  private:

    // IPAddress storage implementation interface
    class Impl
    {
      public:	
        virtual ~Impl() {};

        virtual IPFamily family() const = 0;
        virtual int af() const = 0;
        virtual socklen_t addrLength() const = 0;
        virtual const void* addr() const = 0;
        virtual bool isWildcard() const	= 0;
        virtual bool isLoopback() const = 0;
        virtual bool isBroadcast() const = 0;
        virtual bool isMulticast() const = 0;
        virtual bool isLinkLocal() const = 0;
        virtual bool isSiteLocal() const = 0;
        virtual bool isIPv4Mapped() const = 0;
        virtual bool isIPv4Compatible() const = 0;
        virtual std::string toString() const = 0;

      protected:
        Impl() {};

      private:
        Impl(const Impl&) = delete;
        Impl& operator = (const Impl&) = delete;
    };

    // Base for all IPv4 Address storage implementation
    // Derived class should not add more data members
    class IPv4Impl: public Impl
    {
      public:
        IPFamily family() const override { return IPFamily::IPv4; }
      protected:
        struct in_addr   addr_;
    };

    // List all IPv4Impl derived classes here to be defiened in the cpp file
    class IPv4ImplDefault;

#if ALT_IPV6_AVAILABLE
    // Base for all IPv6 Address storage implementation.
    // Derived class should not add more data members
    class IPv6Impl: public Impl
    {
      public:
        IPFamily family() const override { return IPFamily::IPv6; }
      protected:
        struct in6_addr   addr_;
    };

    // List all IPv6Impl derived classes here to be defiened in the cpp file
    class IPv6ImplDefault;

	static constexpr size_t storage_size = sizeof(IPv6Impl);
#else
	static constexpr size_t storage_size = sizeof(IPv4Impl);
#endif

	union
	{
        std::aligned_storage<storage_size>::type alignaler_;
		char                                     opaque_[storage_size];
	} storage_;

    char* storage() { return storage_.opaque_; }

    Impl* implPtr()
    {
        return reinterpret_cast<Impl*>(storage_.opaque_);
    }

    const Impl* implPtr() const
    {
        return reinterpret_cast<const Impl*>(storage_.opaque_);
    }

    void fromString(const char* addr, size_t length, IPFamily family);

    enum IPBinaryOP { BitAnd=0, BitOr=1, BitXor=2 };
    IPAddress binaryOP (const IPAddress& other, IPBinaryOP op) const;

  public:

    //-----------------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------------
    /// \brief construct a non-routable meta address in given family
    /// \param family the ip family (IPv4 or IPv6)
    explicit IPAddress(IPFamily family = IPFamily::Unset);

    /// \brief construct an address by copying from other
    IPAddress(const IPAddress& other);

    /// \brief construct an address by the given C string
    /// \param addr_str IP address string in either IPv4 (dotted number or hex) or
    /// IPv6 (colon-separated number)
    /// \param length length of the address string. If zero, the length will be
    /// abtained by scanning the terminator of addr_str
    /// \param family family for the ip address to be constructed
    /// \note throws exception when the address cannot be compiled
    IPAddress(
        const char* addr_str,
        size_t length=0,
        IPFamily family = IPFamily::Unset) noexcept(false);

    /// \brief construct an address by the given string
    /// \param addr_str IP address string a string type that has function c_str()
    /// and length()
    /// \param family family for the ip address to be constructed
    /// \note throws exception when the address cannot be compiled
    IPAddress(
        const std::string& addr_str,
        IPFamily family = IPFamily::Unset
    ) noexcept(false)
        : IPAddress(addr_str.c_str(), addr_str.length(), family)
    {}

    /// \brief construct an address from the native one
    /// \param addr the native ip address buffer
    /// \param length the buffer length
    IPAddress(const void* addr, socklen_t length);

    /// \brief construct an address from mask_bits
    /// \param mask_bits number of mask bits for the high-order bits in ip address
    /// \param family the ip family (IPv4 or IPv6)
	IPAddress(unsigned mask_bits, IPFamily family = IPFamily::Unset);

    //-----------------------------------------------------------------------------
    // Address information
    //-----------------------------------------------------------------------------
    /// \return IP Family (IPV4 or IPv6)
    IPFamily family() const { return implPtr()->family(); }

    /// \return IP address family (typically AF_INET or AF_INET6)
    int af() const { return implPtr()->af(); }

    /// \return the size of the storage for the ip address
    socklen_t addrLength() const { return implPtr()->addrLength(); }

    /// \return the pointer of the storage where the ip address is stored
    const void* addr() const { return implPtr()->addr(); }

    /// \return true if the ip address is a wildcard address (non-applicable)
    bool isWildcard() const { return implPtr()->isWildcard(); }

    /// \return true if the ip address is a loopback address (localhost)
    bool isLoopback() const { return implPtr()->isLoopback(); }

    /// \return true if the ip address is a broadcast address
    bool isBroadcast() const { return implPtr()->isBroadcast(); }

    /// \return true if the ip address is a multicast address
    bool isMulticast() const { return implPtr()->isMulticast(); }

    /// \return true if the ip address is local with the network segment
    bool isLinkLocal() const { return implPtr()->isLinkLocal(); }

    /// \return true if the ip address is private within an organization
    bool isSiteLocal() const { return implPtr()->isSiteLocal(); }

    /// \return true if the ip address is mapped from IPv4
    bool isIPv4Mapped() const { return implPtr()->isIPv4Mapped(); }

    /// \return true if the ip address is IPv4 compatible
    bool isIPv4Compatible() const { return implPtr()->isIPv4Compatible(); }

    /// \brief convert ip address to string
    std::string toString() const { return implPtr()->toString(); }

    //-----------------------------------------------------------------------------
    // Address operation
    //-----------------------------------------------------------------------------
#if defined (ALT_CPP_VERSION_20)
    std::strong_ordering operator<=>(const Point&) const
    {
        size_t sz = addrLength();
        size_t sz_other = other.addrLength();
        if (sz != sz_other)
        {
            return sz < sz_other ? std::strong_ordering::less
                                 : std::strong_ordering::greater;
        }
        int res = ::memcmp(addr(), other.addr(), sz);
        return res==0 ? std::strong_ordering::equal
                      : res < 0 ? std::strong_ordering::less
                                : std::strong_ordering::greater;
    }
#else
    bool operator == (const IPAddress& other) const
    {
        size_t sz = addrLength();
        return sz == other.addrLength() && ::memcmp(addr(), other.addr(), sz) == 0;
    }

	bool operator != (const IPAddress& other) const { return *this!=other; }

	bool operator <  (const IPAddress& other) const
    {
        size_t sz = addrLength();
        size_t sz_other = other.addrLength();
        return sz < sz_other ||
               ( sz == sz_other &&
                 ::memcmp(addr(), other.addr(), sz) < 0);
    }

	bool operator <= (const IPAddress& other) const { return !(other < *this); }
	bool operator >  (const IPAddress& other) const { return other < *this; }
	bool operator >= (const IPAddress& other) const { return !(*this < other); }
#endif

	IPAddress operator & (const IPAddress& other) const;
	IPAddress operator | (const IPAddress& other) const;
	IPAddress operator ^ (const IPAddress& other) const;
	IPAddress operator ~ () const;

    size_t hash() const;
};

} // namespace alt


namespace std {
template <>
struct hash<alt::IPAddress>
{
    size_t operator()(const alt::IPAddress& key) const noexcept
    {
        return key.hash();
    }
};
} // namespace std
