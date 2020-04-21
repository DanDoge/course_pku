#include <memory.h>

/* API provided by system */

void ip_DiscardPkt(char * pBuffer ,int type);
void ip_SendtoLower(char *pBuffer ,int length);
void ip_SendtoUp(char *pBuffer, int length);
unsigned int getIpv4Address();

int stud_ip_recv(char * pBuffer, unsigned short length){

    // in case version == 1xxx, which would also yield VERSION_ERROR, but just to be concise
    unsigned int version = ((unsigned int)pBuffer[0]) >> 4;
    if(version != 4){
        ip_DiscardPkt(pBuffer, STUD_IP_TEST_VERSION_ERROR);
        return -1;
    }

    unsigned int ip_head_length = ((unsigned int)pBuffer[0]) & 0xF;
    if(ip_head_length < 5 || ip_head_length > 15){
        // according to RFC791, ip_head_length <= 15
        // see RFC791 section 3.1, mentioned in "total length" part
        // ref: https://tools.ietf.org/html/rfc791
        ip_DiscardPkt(pBuffer, STUD_IP_TEST_HEADLEN_ERROR);
        return -1;
    }

    unsigned int time_to_live = ((unsigned int)pBuffer[8]);
    if(time_to_live == 0){
        ip_DiscardPkt(pBuffer, STUD_IP_TEST_TTL_ERROR);
        return -1;
    }

    unsigned int destination_address = ntohl(*(unsigned int* )(pBuffer + 16)); // might be wrong
    if(destination_address != getIpv4Address() || destination_address != (~0)){
        // if it is necessray to consider
        //     - localhost address: 127.0.0.0/8
        //     - link-local address: 169.254.0.0/16
        // then check: (dst_addr >> 24) != 127 and (dst_addr >> 16) != ((169 << 8) + 254)
        ip_DiscardPkt(pBuffer, STUD_IP_TEST_DESTINATION_ERROR);
        return -1;
    }

    unsigned int header_checksum = 0;
    for(int i = 0; i < 4 * ip_head_length; i += 2){
        // 4 * IHL % 2 == 0, no need to consider odd/even issue
        header_checksum += (pBuffer[i] << 8) + pBuffer[i + 1];
    }
    while(header_checksum >> 16){
        // in case header_checksum == 0x1FFFF
        // add once might not be enough
        // ref: https://tools.ietf.org/html/rfc1071 section 4.1

        // we stick to big-endian byte order, since checksum is byte order independent
        // ref: https://tools.ietf.org/html/rfc1071 section 1.2.(B)
        header_checksum == (header_checksum >> 16) + (header_checksum & 0xFFFF);
    }
    header_checksum = ~header_checksum;

    if(header_checksum != 0){
        ip_DiscardPkt(pBuffer, STUD_IP_TEST_CHECKSUM_ERROR);
        return -1;
    }

    ip_SendtoUp(pBuffer, length);

    return 0;
}

int stud_ip_Upsend(char* pBuffer, unsigned short len, unsigned int srcAddr,
    unsigned int dstAddr ,byte protocol, byte ttl){

    unsigned int version = 4;
    unsigned int ip_head_length = 5; // no option header in this case
    unsigned int total_length = 4 * ip_head_length + len;


    unsigned char* total_buffer = malloc(total_length);
    memset(total_buffer, 0, total_length);

    total_buffer[0] = (version << 4) | ip_head_length;
    // type of service not specified -> set to zero
    total_buffer[2] = total_length >> 8;
    total_buffer[3] = total_length & 0xFF;
    // identification, flags, fragment offset not specified
    total_buffer[8] = time_to_live;
    total_buffer[9] = protocol;

    total_buffer[12] = srcAddr >> 24;
    total_buffer[13] = srcAddr >> 16;
    total_buffer[14] = srcAddr >> 8;
    total_buffer[15] = srcAddr & 0xFF;

    total_buffer[16] = dstAddr >> 24;
    total_buffer[17] = dstAddr >> 16;
    total_buffer[18] = dstAddr >> 8;
    total_buffer[19] = dstAddr & 0xFF;

    unsigned int header_checksum = 0;
    for(int i = 0; i < 20; i += 2){
        header_checksum += (total_buffer[i] << 8) + total_buffer[i + 1];
    }
    while(header_checksum >> 16){
        header_checksum == (header_checksum >> 16) + (header_checksum & 0xFFFF);
    }
    header_checksum = ~header_checksum;
    total_buffer[10] = header_checksum >> 8;
    total_buffer[11] = header_checksum & 0xFF;

    memcpy(total_buffer + 20, pBuffer, len);

    ip_SendtoLower(total_buffer, total_length);

    return 0;
}
