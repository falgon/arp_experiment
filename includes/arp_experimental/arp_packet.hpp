// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_INCLUDES_ARP_PACKET_HPP
#define INCLUDED_INCLUDES_ARP_PACKET_HPP
#include <arp_experimental/detail/config.hpp>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <srook/cstdint.hpp>
#include <srook/algorithm/for_each.hpp>
#include <srook/iterator/range_access.hpp>
#include <srook/string/string_view.hpp>
#include <srook/expected/expected.hpp>
#include <iostream>
#include <numeric>

namespace arp_experimental {
SROOK_INLINE_NAMESPACE(v1)

class arp_packet {
public:
    arp_packet(srook::string_view interface) SROOK_NOEXCEPT_TRUE
        : sock_(init_sock(::socket(AF_INET, SOCK_STREAM, 0)))
    {
        sock_ >>= [this, &interface](int sock) -> srook::expected<int, std::error_code> {
            ::ifreq ifr;
            std::copy_n(srook::begin(interface), IF_NAMESIZE, ifr.ifr_name);
            if (::ioctl(sock, SIOCGIFADDR, &ifr, sizeof(ifr)) < 0) {
                registed_close(sock);
                return {};
            }
            sockaddr_in sin;
            std::memcpy(&sin, &ifr.ifr_addr, sizeof(sockaddr_in));
            ipAddr = ::ntohl(sin.sin_addr.s_addr);
            if (::ioctl(sock, SIOCGIFHWADDR, &ifr, sizeof(ifr)) < 0) {
                registed_close(sock);
                return {};
            }
            ifr_.emplace(srook::move(ifr));
            return {};
        };
    }

    bool send(srook::string::string_view target_ip)
    {
        if (!outerr()) return false;

        int arp_fd = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
        if (arp_fd < 0) return registed_close(sock_.value());
        
        arp_st pkt {}; // target_mac and padding are initialized zero.
#ifdef BOGUS_BROAD_CAST_ADDR
        std::memset(pkt.dst_mac, 0xff, (5 * sizeof(::u_int8_t)));
        std::memset(pkt.dst_mac + 5, 0xfe, sizeof(::u_int8_t));
#else
        std::fill(srook::begin(pkt.dst_mac), srook::end(pkt.dst_mac), 0xff);
#endif
        srook::for_each(srook::algorithm::make_counter(pkt.src_mac), [this](::u_int8_t& src, std::size_t i) {
            std::memset(&src, ifr_.value().ifr_hwaddr.sa_data[i] & 0xff, sizeof(::u_int8_t)); 
        });

        // ref: RFC1700 Assigned Numbers
        pkt.ether_type = htons(0x0806); 
        pkt.hw_type = htons(ARPHRD_ETHER);
        pkt.proto_type = htons(0x0800);
        pkt.hw_size = 0x06;
        pkt.proto_size = 0x04;
        pkt.opcode = htons(ARPOP_REQUEST);
        std::copy(srook::begin(pkt.src_mac), srook::end(pkt.src_mac), srook::begin(pkt.sender_mac));

        pkt.sender_ip = ::htonl(ipAddr);
        pkt.target_ip = ::inet_addr(target_ip.data());

        if (::ioctl(sock_.value(), SIOCGIFINDEX, &ifr_.value(), sizeof(::ifreq)) < 0) 
            return registed_close(arp_fd, sock_.value());

        union sadd {
            sockaddr_ll sal;
            sockaddr sa;
        } sadder;

        sadder.sal.sll_family = AF_PACKET;
        sadder.sal.sll_ifindex = ifr_.value().ifr_ifindex;
        sadder.sal.sll_protocol = htons(ETH_P_ARP);
        sadder.sal.sll_halen = 0;

        if (::sendto(arp_fd, &pkt, sizeof(pkt), 0, &sadder.sa, sizeof(sadder.sal)) < 0) 
            return registed_close(arp_fd, sock_.value());
        
        return true;
    }

    SROOK_CONSTEXPR bool valid() const SROOK_NOEXCEPT_TRUE
    {
        return sock_.valid() && ifr_.valid();
    }
private:
    srook::expected<int, std::error_code> init_sock(int x)
    {
        if (x < 0) return std::error_code(errno, std::generic_category());
        else return x;
    }

    template <class... Close>
    SROOK_FORCE_INLINE bool registed_close(Close&&... cls)
    {
        ifr_.emplace(errno, std::generic_category());
        [](auto&&...){}(::close(srook::forward<Close>(cls))...);
        return outerr();
    }

    SROOK_FORCE_INLINE bool outerr() const
    {
        return oerr(sock_) && oerr(ifr_);
    }

    template <class T>
    SROOK_FORCE_INLINE bool oerr(const T& er) const
    {
        if (er.any_valid() && !er.valid()) {
            std::cerr << er.error().message() << std::endl;
            return false;
        }
        return true;
    }

    struct arp_st {
        // ether header
        ::uint8_t dst_mac[ETH_ALEN], src_mac[ETH_ALEN];
        unsigned short int ether_type;

        // ARP header
        unsigned short int hw_type, proto_type;
        ::u_int8_t hw_size, proto_size;
        unsigned short int opcode;
        ::u_int8_t sender_mac[ETH_ALEN];
        srook::uint32_t sender_ip;
        ::uint8_t target_mac[ETH_ALEN];
        srook::uint32_t target_ip;
        srook::uint8_t padding[18];
    } __attribute__ ((packed));

    srook::expected<int, std::error_code> sock_;
    srook::expected<::ifreq, std::error_code> ifr_;
    unsigned long int ipAddr;
};

SROOK_INLINE_NAMESPACE_END
} // namespace arp_experimental

#endif
