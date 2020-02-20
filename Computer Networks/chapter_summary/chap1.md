### chapter 1

the Internet as a network of networks

hosts and end systems
- end systems connected by a network of communications links and packet switches
    - made up of diff. types of physical media: coaxial cable, copper wire, optical fiber, radio spectrum
    - transmission rate in b/s
    - end systems segments data and add headers to each segment hwen sending data
- packet switches takes a packet on incoming communication links and forwards on one of its outgoing communication links
    - routers and link-layer switches
    - link-layer switches typically used in access networks, routers in network core
    - route/path: sequences of communication links and packet switches traversed by a packet from the sending end system to the receiving end system
- end systems access the Internet through Internet Service Providers(ISP)
    - each ISP network, managed independently, runs the IP protocol
    - request for comments(RFCs)

distributed applications: involve multiple end systems that exchange data with each other
    - API specifies how a program asks the Internet to deliver data to a specific destination

A protocol defines the format and the order of messages exchanged between two or more communicating entities, as well as the actions taken on the transmission and/or receipt of a message or other event.

end systems referred to as hosts, because they host applications programs.

access network: physically connects an end system to the first router

digital subscriber line
- the telco as ISP
- using telephone line to exchange data similtaneously at diff. frequencies

cable Internet access
- make use of cable tv infra.
- shared broadcast system: every packet travels on every link to every home

fiber to the home
- shared by many homes until the fiber gets close to homes

Wireless LAN access based on IEEE 802.11, WiFi

guided media / unguided media
- guided along a solid medium
- propagate in the atmosphere / outer space

store-and-forward transmission
- used by most packet switches
- receive the entire packet before it can begin to transmit the first bit onto the outbound link
- delay will be L / R
- for each attached link, packet switch has an output buffer, stores packets the router is about to send to that link
    - queuing delay
    - packet loss when the buffer is completely full
- special routing protocols are used to automatically set the forwarding tables

circuit switching
- a path are reserved for the duration of communication session between end systems
- frequency-division multiplexing or time-division multiplexing
    - some circuits may be idle during silnet periods

network of networks
- the access ISPs must be interconnected

delay
- nodal processing delay, queuing delay, transmission delay, propagation delay -> sum up to total nodal delay
- processing delay: examine the packet's header and determine where to direct the packet
- transmission delay: L / R, time required to pus hall of the packets bits into the link
- propagation delay: time required to propagate from the beginning of the link to the other router
- queuing delay
    - statistical measures are used: average queuing delay / var. of queuing delay, prob. of queuing delay exceeds some value
    - traffic intensity: La/R, if LA/R > 1, queueing delay will go to infty
        - in practice, the router will drop that packet
- delay will also occur as a part of protocal

throughput
- min of all the rate of intermediate links
- the core of the Internet is over-provisioned with high speed links that experience little congestion
    - more generally, it also depends on the intervening traffic

layer model
- each layer provide its service by
    - performing certain actions with in that layer
    - using services of the layer directly below it
- service model: the service a layer offers to the layer above
- protocal stack
    - the Internet has two transport protocals, TCP and UDP
        - TCP breaks long messages into shorter segments, provide a congestion-control mechanism
        - UDP provides a connectionless service, no reliability, no flow control, and no congestion control
    - only one IP protocal in the world in network layer, while many routing protocal are in that layer
    - link layer: Ethernet, WiFi, cable access network's DOCSIS protocol
    - physical layer: depends on the actual transmission medium of the link
- OSI model: 7 layers
- header fields, payload field
