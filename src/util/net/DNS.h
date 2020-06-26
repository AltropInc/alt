//**************************************************************************
// DNS.h
// Library: alt_util
//
// Copyright (c) 2021-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************



#pragma once
#include "SocketAddress.h"              // for SocketAddress

#include <util/Defs.h>                  // for ALT_UTIL_PUBLIC
#include <util/types/TemplateHelper.h>  // for MOVEONLY

#include <string>                       // for string
#include <unordered_map>                // for unordered_map

namespace alt
{

/**
 * \struct DNS
 * \ingroup NetUtil
 * \brief A DNS sever to resolve ip address from domain names
 */
class ALT_UTIL_PUBLIC DNS
{
  public:
    struct HostInfo
    {
      public:

        MOVEONLY(HostInfo);
        HostInfo(const void* ai_list);
        ~HostInfo();

        /// \brief gets host name
        const char* hostName();

        /// \brief gets socket address for this host
        /// \param family the hind for which IP address family is desired. If it is given
        /// IPFamily::Unset, IP address family will be resolved either in IPv4 or IPv6
        /// \return the socket address for the given IP family
        const sockaddr_storage* getSocketAddress(IPFamily family = IPFamily::IPv4) const;

#if ALT_IPV6_AVAILABLE
        /// \return true if IPv6 address is available
        bool IPv6Available() const;
#endif

      private:
        void swap(HostInfo& other) { std::swap(ai_list_, other.ai_list_); }

        // points to opaque address info list which is implementation depdent
        const void* ai_list_ {nullptr};
    };

    /// \brief returns a singleton of DNS
    static DNS& instance();

    /// \brief host info (socket addresses) from host name
	const HostInfo& getHostInfo(const std::string& name);

  private:
    DNS() {};
    std::unordered_map<std::string, HostInfo> host_info_map_;
};


} // namespace alt


