### link layer

from book *Computer Networks a Top Down Approach*

two types of link-layer channels
- broadcast channels
- point-to-point communication link

node: any device runs a link-layer protocal

link: communication channels connect adjacent nodes
- a datagram has to move over several links
    - encapsulated in a link-layer frame

services of the link layer
- framing: specify structure of frames
- link access: medium access control protocal of how a frame is transmitted onto link
- reliable delivery: errorfree transmission, ack and retransmission
- error detection and correction
    - still may be undetected bit errors

parity check
- detect 1 bit error
- two-dementional parity: correct one bit error
    - detect two bit error
- forward error correction
    - decrease # of retransmission, correct errors at receiver

CRC
- sender adn receiver agree on an r+1 bit pattern: generator

muitiple access links
- coordinate access of multiple sending and receiving nodes
    - muitlple frames may collide at all of the receivers
    - and all the frames are lost, since signals are entangled
- channel partitioning protocal
    - max. datarate at R/N
- random access protocal
    - when collision, resend teh frame after random interval, until no collision
    - max. efficiency at 1/e
- take turns protocal
    - vote a master node / pass a token

MAC address
- 6 bytes, unique for each device in the world
    - managed by the IEEE
- ARP
    - what is one device's MAC address with its IP being XXX?
    - only for a subnet
    - ARP table
    - ARP query if lack an entry -> automatically set up the ARP table
    - send frame to another subnet -> send to the router

VLAN
- some of the ports form a broadcast domain
    - the switch and the router are combined together
- VLAN truck
    - connect two switches using port shared by all VLANs

wireless network
- wireless hosts
- wireless links
- base station: responsible fr sending and receiving data to/from a wireless host
    - cell towers, access points
    - connected to larger networks
- network infrastructure

no changes needed at/above network layer
- diff. btn. wired and wireless network: link layer
    - decreasing signal strength, interference from other sources, multipah propagation
    - more common bit errors: CRC error detection codes, reliable data-transfer protocols
- BER, SNR, modulation scheme
- hidden terminal problem
- CDMA: value received being the sum of all senders
    - carefully chosen CDMA codes
    - assume signal strength from all senders are the same

802.11
- a/b/g
- basic service set
- ad hoc network
- service set identifier
- beacon frames: SSID and MAC address
    - passive/active scanning(second request/response needed)
- CSMA/CA
    - cannot do CD in wireless network
    - transmit a whole frame once transmission begins
    - send back ack, if received frame passes CRC
    - random backoff value: dec. only when channel is free
    - RTS/CTS
- frame
    - address1: mac of wireless station receives the frame
    - address2: mac of wireless station sends the frame
    - address3: mac of router
    - since router is not aware of AP
- mobility in the same IP subnet: disassociate/associate with AP at link layer level, TCP connections maintained
