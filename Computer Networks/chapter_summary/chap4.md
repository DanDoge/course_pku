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

routing
- a host is attached directly to the default/first-hop router
- routing alrorithm finds a good(least cost) path from source to destination
    - global routing algorithm: using complete, global knowledge about the network obtained before performing the calculation
    - decentralized routing algorithm: carried out in a iterative, distributed way, exchange information with neighboring nodes
- link-state algorithm: network topology and link costs are known
    - Dijkstra
    - oscillation: using a congestion/delay based link metric, all traffic move to the same direction, and cause congestion, and all move to the other direction
        - randomize time sends a link advertisement
- distance-vector algorithm
    - each node receive information from directly neighbor, and distributes results to neighbors(only when results changed), asynchronous in nature
    - Bellman-Ford update equation
    - good news propagated quickly while bad news not(routing loop)
        - add poisoned reverse: advertise d(x, y) = infty to z if x forward packets to y through z
- RIP
    - a distance vector protocol
    - a hop count as cost metric, amximum cost of a path is 15
    - routing updates exchanged every 30 seconds using RIP response message
    - neighbor becomes not reachable if not heard once in 180 seconds
- OSPF
    - locally runs Dijkstra with itself as the root node
    - broadcast routing information to all other routers
- BGP
    - inter AS routing protocol

broadcast and multicast
- broadcast: from source to all other nodes
    - N-way-unicast: how can sender know all receivers' address?
    - uncontrolled flooding: loop in cycles
    - controlled flooding   
        - maintain a list of source addr and seq number, if received a packet in this list, drop, or
        - RPF: transmit packet only when received from the shortest path from the source
        - spanning-tree broadcast: avoid transmission of redundant packets, put all incident links belonging to the spanning tree
- multicast: from source to a sub set of nodes
    - a single identifier for a group of receivers: class D multicast IP addresses
    - IGMP: for a host to inform router that an app wants to join a multicast group
        - membership_query: router to all hosts
        - membership_report: host to router
        - leave_group: optional, otherwise timeout
    - routing
        - group-shared tree: a single tree for all senders
        - source-based tree: a tree for each source
