#include "DNS.h"

#include <util/system/SysError.h>       // for SYS_ERR_THROW
#include <util/string/StrScan.h>        // For StrParser

#include <util/system/SysError.h>       // for SYS_ERR_THROW
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>                      // for getaddrinfo, etc.
#include <string.h>                     // for memset

namespace alt
{

DNS& DNS::instance()
{
    static DNS s_dns;
    return s_dns;
}

DNS::HostInfo::HostInfo(const void* ai_list)
    : ai_list_(ai_list)
{}

DNS::HostInfo::~HostInfo()
{
    if (ai_list_)
    {
        freeaddrinfo(const_cast<addrinfo*>(reinterpret_cast<const addrinfo*>(ai_list_)));
    }
}

const char* DNS::HostInfo::hostName()
{
    return ai_list_ ? reinterpret_cast<const addrinfo*>(ai_list_)->ai_canonname : nullptr;
}

const sockaddr_storage* DNS::HostInfo::getSocketAddress(IPFamily family) const
{
	for (auto ai = reinterpret_cast<const addrinfo*>(ai_list_); ai; ai = ai->ai_next)
	{
		if (ai->ai_addrlen && ai->ai_addr)
		{
			switch (ai->ai_addr->sa_family)
			{
			case AF_INET:
                if (family != IPFamily::IPv6)
                {
                    return reinterpret_cast<const sockaddr_storage*>(ai->ai_addr);
                }

#if ALT_IPV6_AVAILABLE
			case AF_INET6:
                if (family != IPFamily::IPv4)
                {
                    return reinterpret_cast<const sockaddr_storage*>(ai->ai_addr);
                }
#endif
			}
		}
	}
    return nullptr;
}

bool DNS::HostInfo::IPv6Available() const
{
	for (auto ai = reinterpret_cast<const addrinfo*>(ai_list_); ai; ai = ai->ai_next)
	{
		if (ai->ai_addrlen && ai->ai_addr && ai->ai_addr->sa_family == AF_INET6)
		{
			return true;
		}
	}
    return false;
}

void punyEncode(const std::string& uft8_src, std::string& punnycode_string)
{
    std::u32string src = toU32String(uft8_src);
    size_t srclen = src.length();
        
    // Copy all ASCII codes to dst
    for (auto ch: src)
    {
        if (ch < 128)
        {
            punnycode_string.push_back(char(ch));
        }
    }

    size_t processed = punnycode_string.length();
    size_t b = processed;

    // put delimiter '-' if any ASCII code copied
    if (processed > 0)
    {
        punnycode_string.push_back('-');
    }

    // Punycode bootstring parameters
    enum
    {
        BASE = 36,
        TMIN = 1,
        TMAX = 26,
        SKEW = 38,
        DAMP = 700,
        INITIAL_BIAS = 72,
    };

    char32_t bias  = INITIAL_BIAS;
    char32_t delta = 0;
    char32_t max_non_ascii          = std::numeric_limits<char32_t>::max();
    char32_t min_non_ascii          = 128;
    char32_t next_non_ascii         = max_non_ascii;
    char32_t smallest_non_ascii     = min_non_ascii;

    for (; processed < srclen; smallest_non_ascii++, delta++)
    {
        // All non-ascii code < smallest_non_ascii have been processed, find the next non-ascii
        // that is smallest but larger than current smallest_non_ascii
        for (size_t si = 0; si < srclen; si++)
        {
            if (src[si] >= smallest_non_ascii && src[si] < next_non_ascii)
            {
                next_non_ascii = src[si];
            }
        }

        if ((next_non_ascii - smallest_non_ascii) > (max_non_ascii - delta) / (processed + 1))
        {
            // this is unlikely
            SYS_ERR_THROW(NetException, "punycode encode  overflow", false);
        }

        delta += (next_non_ascii - smallest_non_ascii) * (processed + 1);
        smallest_non_ascii = next_non_ascii;

        for (size_t si = 0; si < srclen; si++)
        {
            if (src[si] < smallest_non_ascii)
            {
                if (++delta == 0)
                {
                    // this is unlikely
                    SYS_ERR_THROW(NetException, "punycode encode overflow", false);
                }
            }
            else if (src[si] == smallest_non_ascii)
            {
                // encode_digit: 0..25 map to ASCII a..z or A..Z and 26..35 map to ASCII 0..9
                auto encode_digit = [] (char32_t c)->char { return c > 25 ? c + 22 : c + 'a'; };

                // use delta as generalized variable-length integer
                char32_t k = BASE;
                char32_t q = delta;
                while (true)
                {
                    size_t t = k <= bias ? TMIN
                                         : k >= bias + TMAX ? TMAX
                                                            : k - bias;
                    if (q < t)
                    {
                        break;
                    }

                    punnycode_string.push_back(encode_digit(t + (q - t) % (BASE - t)));
                    q = (q - t) / (BASE - t);
                    k += BASE;
                }
                punnycode_string.push_back(encode_digit(q));

                // adapt bias
                delta /= processed == b ? DAMP : 2;
                delta += delta / (processed + 1);
                for (k = 0; delta > ((BASE - TMIN) * TMAX) / 2; k += BASE)
                {
                    delta /= (BASE - TMIN);
                }
                bias = k + (BASE - TMIN + 1) * delta / (delta + SKEW);

                delta = 0;
                processed++;
            }
        }
    }
}

const std::string& encodeHostName(const std::string& name, std::string& result)
{
	std::string::const_iterator it = name.begin();
	std::string::const_iterator end = name.end();
	while (it != end)
	{
		std::string::const_iterator label_start = it;
		bool is_basic = true;
		while (it != end && *it != '.')
		{
			if (static_cast<unsigned char>(*it++) >= 128) is_basic = false;
		}

		if (is_basic)
        {
            // if result is empty, so far all parsed sections are in basic strings
            if (!result.empty())
            {
                result.append(label_start, it);
            }
        }
        else
        {
            // We have to encode and use the result string. First, push to result
            // all sections in basic strings parsed before
            if (result.empty())
            {

                result.append(name.begin(), label_start);
            }

            // then push encode string for tis section
            {
                result.append("xn--");
                std::string encoded;
                std::string label(label_start, it);
                punyEncode(label, encoded);
                result.append(encoded);
            }
        }
		if (*it != '.')
        {
            if (!result.empty()) result.push_back('.');
            ++it;
        }
	}
	return result.empty() ? name : result;
}

const DNS::HostInfo& DNS::getHostInfo(const std::string& name)
{
    auto it = host_info_map_.find(name);
    if ( it!=host_info_map_.end() )
    {
        return it->second;
    }

	struct addrinfo hints;
	struct addrinfo *ai_list;
	::memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG;

    std::string encoded_name;

	int rc = getaddrinfo(encodeHostName(name, encoded_name).c_str(), nullptr, &hints, &ai_list);
	if (rc != 0)
	{
        SYS_ERR_THROW(NetException);
    }

    auto [iter, inserted] = host_info_map_.emplace(name, HostInfo(ai_list));
    return iter->second;
}


}