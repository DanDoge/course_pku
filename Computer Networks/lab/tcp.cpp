/* global variables */
int gSrcPort = 2005;
int gDstPort = 2006;
int gSeqNum = 0;
int gAckNum = 0;
int global_socket_number = 0;

/* states for TCP automata */
typedef enum{TCP_closed, TCP_synsent, TCP_established, TCP_finwait1, TCP_finwait2, TCP_timewait} TCP_State;

/* APIs */
void tcp_DiscardPkt(char * pBuffer, int type);
void tcp_sendIpPkt(unsigned char* pData, uint16 len, unsigned int srcAddr,
unsigned int dstAddr, uint8 ttl);
int waitIpPacket(char *pBuffer, int timeout);
UINT32 getIpv4Address( );
UINT32 getServerIpv4Address( );

struct TCB{
    unsigned int source_address;
    unsigned int destination_address;
    unsigned int source_port;
    unsigned int destination_port;
    unsigned int seq;
    unsigned int ack;
    unsigned int socket_number;
    TCP_State state;

    TCB(){
        socket_number = global_socket_number++;
        seq_number = gSeqNum++;
        state = TCP_closed;
        srcPort = gSrcPort++;
    }
}

TCB* tcb_cache; // tmp tcb to pass some info from socket functions to tcp functions
map<int, TCB> socket2tcb;

int stud_tcp_input(char *pBuff, unsigned short len, unsigned int srcAddr,
unsigned int dstAddr){
    // where is document for tcp_sendReport function?
    /* check checksum */
    unsigned int header_checksum = 0;
    for(int i = 0; i < len + 20; i += 2){
        header_checksum += (pBuff[i] << 8) + pBuff[i + 1];
    }
    while(header_checksum >> 16){
        header_checksum == (header_checksum >> 16) + (header_checksum & 0xFFFF);
    }
    header_checksum = ~header_checksum;
    if(header_checksum != 0){
        // or call tcp_DiscardPkt()?
        return -1;
    }
    /* convert byte order: ...of what? */

    /* check seq number */
    unsigned int ack_number = *(int* )(pBuff + 8);
    if(ack_number != tcb->seq){
        tcp_DiscardPkt(pBuff, STUD_TCP_TEST_SEQNO_ERROR);
        return -1;
    }

    /* automata, update state? */
    unsigned int seq_number = *(int* )(pBuff + 4);
    tcb_cache->ack = seq_number + 1;
    tcb_cache->seq = ack_number;

    if(tcb_cache->state == TCP_synsent){
        /* what are src and dst port? */
        tcb_cache->state = TCP_established;
        stud_tcp_output(NULL, 0, PACKET_TYPE_ACK, gSrcPort, gDstPort, getIpv4Address(), getServerIpv4Address());
    }else if(tcb_cache->state == TCP_finwait1){
        tcb_cache->state = TCP_finwait2;
    }else if(tcb_cache->state == TCP_finwait2){
        tcb_cache->state = TCP_timewait;
        stud_tcp_output(NULL, 0, PACKET_TYPE_ACK, gSrcPort, gDstPort, getIpv4Address(), getServerIpv4Address());
    }
    return 0;
}

void stud_tcp_output(char *pData, unsigned short len, unsigned char flag,
unsigned short srcPort, unsigned short dstPort, unsigned int srcAddr, unsigned int
dstAddr){
    /* construct TCP head */
    char* head = malloc(20 + len); // no options
    (short* )head[0] = srcPort;
    (short* )head[1] = dstPort;
    (int* )head[1] = tcb_cache->seq;
    (int* )head[2] = tcb_cache->ack;
    head[13] = 0x50; // first 4-bits being header length (in words)
    head[14] = flag;
    (short* )head[8] = 1; // windowsize: 1
    memcpy(head + 20, pData, len);

    unsigned int header_checksum = 0;
    for(int i = 0; i < len + 20; i += 2){
        header_checksum += (head[i] << 8) + head[i + 1];
    }
    while(header_checksum >> 16){
        header_checksum == (header_checksum >> 16) + (header_checksum & 0xFFFF);
    }
    header_checksum = ~header_checksum;
    (short* )head[9] = header_checksum;

    tcp_sendIpPkt(head, 20 + len, srcAddr, dstAddr, 64); // ttl not specified

    if(flag == PACKET_TYPE_SYN && tcb_cache->state == TCP_closed){
        tcb_cache->state = TCP_synsent;
    }else if(flag == PACKET_TYPE_FIN_ACK && tcb_cache->state == TCP_established){
        tcb_cache->state = TCP_finwait1;
    }
    return ;
}

int stud_tcp_socket(int domain, int type, int protocol){
    TCB* tmp_tcb = new TCB();
    socket2tcb.insert(pair<int, TCB>(tmp_tcb->socket_number, *tcb));
    return tmp_tcb->socket_number;
}

int stud_tcp_connect(int sockfd, struct sockaddr_in* addr, int addrlen){
    map<int, TCB>::iterator it = socket2tcb.find(sockfd);
    tcb_cache = it->second;
    tcb_cache->srcAddr = getIpv4Address();
    tcb_cache->dstAddr = addr->?; // what is a socket structure pointer?
    tcb_cache->dstPort = addr->?; // same as above

    stud_tcp_output(NULL, 0, PACKET_TYPE_SYN, tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);

    char* head = malloc(200);
    int tmp = -1;
    while(tmp == -1){
        tmp = waitIpPacket(head, 1);
    }
    if(tmp[14] == PACKET_TYPE_SYN_ACK){
        tcb_cache->seq = *(int* )head[1];
        tcb_cache->ack = *(int* )head[2] + 1;
        stud_tcp_output(NULL, 0, PACKET_TYPE_ACK, tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);
        tcb_cache->state = TCP_established;
        return 0;
    }else{
        return -1;
    }
    return -1;
}

int stud_tcp_send(int sockfd, const unsigned char* pData, unsigned short datalen,
int flags){
    /* what does "this function will send 'this is a tcp test' to server" mean?
     * should we ignore pData?
     */
    map<int, TCB>::iterator it = socket2tcb.find(sockfd);
    tcb_cache = it->second;
    if(tcb_cache->state == TCP_established){
        stud_tcp_output(pData, datalen, PACKET_TYPE_DATA, tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);

        char* head = malloc(200);
        int tmp = -1;
        while(tmp == -1){
            tmp = waitIpPacket(head, 1);
        }
        if(tmp[14] == PACKET_TYPE_ACK){
            if((int* )head[2] != tcb_cache->seq + datalen){
                tcp_DiscardPkt(head, STUD_TCP_TEST_SEQNO_ERROR);
                return -1;
            }
            tcb_cache->seq = (int* )head[2];
            tcb_cache->ack = (int* )head[1] + datalen;
            return 0;
        }
        return -1;
    }
    return -1;
}

int stud_tcp_recv(int sockfd, const unsigned char* pData, unsigned short datalen,
int flags){
    map<int, TCB>::iterator it = socket2tcb.find(sockfd);
    tcb_cache = it->second;
    if(tcb_cache->state == TCP_established){
        char* head = malloc(200);
        int tmp = -1;
        while(tmp == -1){
            tmp = waitIpPacket(head, 1);
        }
        memcpy(pData, head + 20, sizeof(head) - 20);
        // how to send ack(level4 packet) through sendIpPkt(level3 datagram)?
        stud_tcp_output(NULL, 0, PACKET_TYPE_ACK, tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);
        return 0;
    }
    return -1;
}

int stud_tcp_close( int sockfd ){
    map<int, TCB>::iterator it = socket2tcb.find(sockfd);
    tcb_cache = it->second;

    if(tcb_cache->state == TCP_established){
        tcb_cache->state = TCP_finwait1;
        stud_tcp_output(NULL, 0, PACKET_TYPE_FIN_ACK, tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);
        char* head = malloc(200);
        int tmp = -1;
        while(tmp == -1){
            tmp = waitIpPacket(head, 1);
        }
        if(head[14] == PACKET_TYPE_ACK){
            tcb_cache->state = TCP_finwait2;
            int tmp = -1;
            while(tmp == -1){
                tmp = waitIpPacket(head, 1);
            }
            if(head[14] == PACKET_TYPE_FIN_ACK){
                tcb_cache->seq = (int* )head[2];
                tcb_cache->ack = (int* )head[1]++;
                stud_tcp_output(NULL, 0, PACKET_TYPE_ACK, , tcb_cache->srcPort, tcb_cache->dstPort, tcb_cache->srcAddr, tcb_cache->dstAddr);
                tcb_cache->state = TCP_timewait;
                return 0;
            }
            return -1;
        }
        return -1;
    }
    return -1;
}
