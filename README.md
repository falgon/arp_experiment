# ARP experiment
[![Build Status](https://travis-ci.org/falgon/arp_experiment.svg?branch=master)](https://travis-ci.org/falgon/arp_experiment)

This is a project for learning to detect computers operating in promiscuous mode 
on the same network using ARP packet and broadcast mechanism.

## Requires
* Linux 4.13.0-37-generic
* gcc 7.2.0
* [Srook C++ Libraries](https://github.com/falgon/SrookCppLibraries)

## build & run
```sh
$ make 
$ sudo ./dst/dump_arp <interface> # receive arp packet
$ sudo ./dst/sendarp <interface> <target ip> # sending arp packet (The destination MAC address is set to ff:ff:ff:ff:ff:ff.)

$ make -f Makefile_bogus
$ sudo ./dst/dump_arp <interface> # receive arp packet
$ sudo ./dst/sendarp <interface> <target ip> # sending arp packet (The destination MAC address is set to ff:ff:ff:ff:ff:fe.)
```

## Related article(Japanese)
* [ARP パケットに対する挙動からネットワーク上の盗聴者を特定する](https://falgon.github.io/roki.log/posts/2018/%205月/01/detectPromiscuous/)

## License
[MIT](./LICENSE)
