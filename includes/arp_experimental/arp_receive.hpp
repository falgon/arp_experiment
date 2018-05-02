// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_INCLUDES_ARP_RECEIVE_HPP
#define INCLUDED_INCLUDES_ARP_RECEIVE_HPP

#include <arp_experimental/detail/config.hpp>
#include <linux/if.h>
#include <srook/config/feature/nested_namespace.hpp>
#include <srook/config/feature/inline_namespace.hpp>
#include <srook/iterator/ostream_joiner.hpp>
#include <srook/string/string_view.hpp>
#include <srook/optional.hpp>
#include <srook/type_traits/is_static_castable.hpp>
#include <srook/type_traits/is_preincrementable.hpp>
#include <srook/range/adaptor/transformed.hpp>
#include <srook/algorithm/copy.hpp>
#include <srook/process/perror.hpp>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace arp_experimental {
SROOK_INLINE_NAMESPACE(v1)

srook::optional<int> initialize_raw_socket(srook::string_view device, bool b) SROOK_NOEXCEPT_TRUE
{
    union address { 
        ::sockaddr_ll sal;
        ::sockaddr a;
    } addr {};
    ::ifreq ifr {};
    int soc = -1;

    if ((soc = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
        srook::process::perror("socket");
        return srook::nullopt;
    }

    std::size_t devsize = device.size() < sizeof(ifr.ifr_name) - 1 ? device.size() : sizeof(ifr.ifr_name) - 1;
    std::copy_n(device.cbegin(), srook::move(devsize), ifr.ifr_name);

    if (::ioctl(soc, SIOCGIFINDEX, &ifr) < 0) {
        srook::process::perror("ioctl");
        ::close(soc);
        return srook::nullopt;
    }
    addr.sal.sll_family = PF_PACKET;
    addr.sal.sll_protocol = htons(ETH_P_ARP);

    addr.sal.sll_ifindex = ifr.ifr_ifindex;
    if (::bind(soc, &addr.a, sizeof(addr.sal)) < 0) {
        srook::process::perror("bind");
        ::close(soc);
        return srook::nullopt;
    }

    if (b) {
        if (::ioctl(soc, SIOCGIFFLAGS, &ifr) < 0) {
            srook::process::perror("ioctl");
            ::close(soc);
            return srook::nullopt;
        }
        ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC;
        if (::ioctl(soc, SIOCSIFFLAGS, &ifr) < 0) {
            srook::process::perror("ioctl");
            ::close(soc);
            return srook::nullopt;
        }
    }

    return { soc };
}

struct show_ether_header {
    SROOK_FORCE_INLINE SROOK_CONSTEXPR SROOK_EXPLICIT show_ether_header(const ::ether_header& eh) SROOK_NOEXCEPT_TRUE
        : eh_(eh) {}
private:
    const ::ether_header& eh_;

    std::string macaddr_n2a(const ::u_char* hwaddr) const
    {
        std::stringstream ostr;
#if SROOK_CPP_LAMBDAS
        const auto l = [] (::u_char c) -> int { return static_cast<int>(c); };
#else
        struct l_ {
            SROOK_CONSTEXPR SROOK_FORCE_INLINE int operator()(::u_char c) const SROOK_NOEXCEPT_TRUE { return static_cast<int>(c); }
        } l;
#endif
        srook::algorithm::copy(srook::string::basic_string_view<::u_char>(hwaddr, ETH_ALEN) | srook::range::adaptors::transformed(srook::move(l)), srook::make_ostream_joiner(ostr << std::hex, ":"));
        return ostr.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const show_ether_header& this_)
    {
        os << "\tdhost = " << this_.macaddr_n2a(this_.eh_.ether_dhost) << '\n';
        os << "\tshost = " << this_.macaddr_n2a(this_.eh_.ether_shost) << '\n';
        os << "\ttype = " << std::hex << ntohs(this_.eh_.ether_type);
        os << std::dec;

        switch (ntohs(this_.eh_.ether_type)) {
            case ETH_P_ARP:
                os << "(ARP)\n";
                break;  
            default:
                os << "(not ARP)\n";
                break;
        }
        return os;
    }
};

SROOK_INLINE_NAMESPACE_END
} // namespace arp_receive

#endif
