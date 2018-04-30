// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_RLROUTER_DETAIL_CONFIG_HPP
#define INCLUDED_RLROUTER_DETAIL_CONFIG_HPP

#if defined(__linux__) && defined(__GNUC__)
#    include <arpa/inet.h>
#    include <net/ethernet.h>
#    include <netinet/if_ether.h>
#    include <netpacket/packet.h>
#    include <sys/ioctl.h>
#    include <sys/socket.h>
#    include <unistd.h>
#else
#    error "This feature needs to linux headers and the gcc compiler."
#endif

#endif
