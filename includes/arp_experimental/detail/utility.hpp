// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_INCLUDES_ARP_EXPERIMENTAL_DETAIL_UTILITY_HPP
#define INCLUDED_INCLUDES_ARP_EXPERIMENTAL_DETAIL_UTILITY_HPP

#include <arp_experimental/detail/config.hpp>
#include <linux/if.h>
#include <srook/config/feature/nested_namespace.hpp>
#include <srook/config/feature/inline_namespace.hpp>
#include <srook/config/feature/current_function.hpp>
#include <srook/optional.hpp>
#include <srook/process/perror.hpp>

namespace arp_experimental {
SROOK_INLINE_NAMESPACE(v1)

namespace detail {

SROOK_FORCE_INLINE void error_close(const char* s, int soc) SROOK_NOEXCEPT(srook::process::perror(s))
{
    srook::process::perror(s);
    ::close(soc);
}

SROOK_FORCE_INLINE srook::optional<int> ioctl(int soc, int flag, ::ifreq* ifr) SROOK_NOEXCEPT_TRUE
{
    return ::ioctl(soc, flag, ifr) < 0 ? error_close(SROOK_CURRENT_FUNCTION, soc), srook::nullopt : srook::make_optional(soc);
}

SROOK_FORCE_INLINE srook::optional<int> socket(int packet_type, int sock_type, ::uint16_t ht) SROOK_NOEXCEPT_TRUE
{
    int soc = ::socket(packet_type, sock_type, ht);
    return soc < 0 ? error_close(SROOK_CURRENT_FUNCTION, soc), srook::nullopt : srook::make_optional(soc);
}

SROOK_FORCE_INLINE srook::optional<int> bind(int soc, const ::sockaddr* addr, ::socklen_t addrlen) SROOK_NOEXCEPT_TRUE
{
    return ::bind(soc, addr, addrlen) < 0 ? error_close(SROOK_CURRENT_FUNCTION, soc), srook::nullopt : srook::make_optional(soc);
}

} // namespace detail

SROOK_INLINE_NAMESPACE_END
} // namespace arp_experimental
#endif
