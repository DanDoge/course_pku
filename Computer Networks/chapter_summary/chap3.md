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
