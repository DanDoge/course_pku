### network layer

from *computer networks a top down approach*

network layer in wvery host and
- forwarding: transfer a packet from incoming link to outcoming link
- routing: all routers interact via routing protocol to determine paths from source to destination

forwarding table: for datagram and vc
- decided by routing algorithm

the Internet's network layer
- best-effort sesrvice: no services at all
    - no ordering preservation, congestion control, loss guarantee

virtual circuit
- a path, vc number **s**, entries in the forwarding table
- routers maintain connection state information: in/outcoming port/vc number

datagram
- each router has a forwarding table: maps destination address to link interfaces
- longest prefix matching rule

router
- input processing: forwarding table has a shadow copy at each input port, copied through a separate bus
    - and do physical and link layer processing, version number, ttl, checksum, update counters
- switch
    - via memory, bus, crossbar
    - random early detection: refer to slides
    - head of line blocking: blocked by another packet at the head of the line

IP: ipv4 and ipv6
- 20-byte header for a typical ip datagram
- length: total length of the ip datagram, in bytes
- length > mtu? fragmentation
    - identification, offset(in 8bytes: alignment needed), flagbit(suggest its the end)
- ip address associated with an interface
    - routers optionally forward message to 255.255.255.255 to neighboring subnets
- dhcp: using udp
    - server discovery, server offer, dhcp request, dhcp ack
- nat
- icmp
    - above ip
    - router discards packet with ttl=0, and sends back a icmp warning
- ipv6
    - options will be lost if ipv6 packet pass through an ipv4 subnet: tunnel
