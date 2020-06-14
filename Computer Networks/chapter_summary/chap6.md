### application layer

application architecture: dictates how the application is structured over various end systems
- client-server architecture
- P2P architecture: self-scalable

process send message to, and receive from a software interface called socket
- bandwidth-sensitive application: one that has throughput requirements
- elastic application: make use of much/little throughput

HTTP
- stateless protocol: server maintain no information about clients
- non-persistent connections / persistent connections: separate or same TCP connections
- request line / header line
- set-cookie header: a id number
    - saved locally in cookie file
- web caching: reducing traffic
    - CDN
    - conditional GET to keep cache up to date

FTP
- a control connection and a data connection
- server maintain state about the user

mail
- user agent, server, SMTP
- mail access protocol: POP3 or IMAP or HTTP

DNS
- translating host name to ip, and host aliasing, mail server aliasing, load distribution
- recursive requiries and iterative queries
