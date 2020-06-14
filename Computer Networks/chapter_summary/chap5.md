### transport layer

transport layer provides logical communication between processes on different hosts
    - implemented in end systems
    - two protocols: UDP, TCP
        - whereas IP is a unreliable service, host-to-host delivery
        - thus the transport layer needs to provide multiplexing and demultiplexing
        - indicated by src and dst port numbers, 0-1023 well-known
        - client side randomly choose a port number whereas server side has to specify a port number
        - TCP socket identified by src/dst port/address

UDP
- provide finer application-level control over what/when is sent
- no delay ot eatablish a connection
- 8 bytes of overhead
    - src port, dst port, length, checksum
    - transport layer error detection is a must, on end-end basis
- used in DNS and RIP

reliable data transfer
- no data are corrupted or lost, and delivered in right order
- sequence number
- window size smaller than or equal to half of the sequence space

TCP
- connection-oriented
- full-duplex service
    - multicasting is not possible with TCP
- hand-shake: three-way
    - send a segment with SYN 1
    - receive a segment with SYN 1
    - send a segment with SYN 0
- end connection
    - send a segment with FIN 1
    - ack
    - server send a segment with FIN 1
    - ack, with timer waiting for other packets
- maximum segment size determined by MTU
- each side has its own send and receive buffer
- header
    - source port, dest port
    - sequence number(number of the first byte in the segment, choose randomly)
    - ack number(number of next byte receiver expects to receive, cumulative acknowledgements)
    - tags(including length(in words)), receive window
    - checksum, urgent data pointer
    - options
- timer -> estimate RTT
    - exponential average
    - and estimate variance
- retransmissions: timer grows exponentially
    - fast retransmit if three duplicate ack received
- flow control
    - sender maintain a var: receive window
        - the receiver told the sender its window
        - when blocked: sender send one byte message to receiver

congestion control
- congestion: queuing delay, retransmissions, waste of bandwidth, especially the packet eventually get dropped
- TCP congestion control
    - end-to-end congestion control
    - maintain a congestion window
        - slow start: exponential growth
            - timeout: thresh = window, window->1
            - triple ack: thresh = window, window/=2
        - congestion avoidance
            - timeout: thresh = window, window=1
            - triple ack: thresh = window, window/=2
        - fast recovery
            - then back to congestion avoidance if no timeout event
    - additive-increase, multiplicative-decrease
