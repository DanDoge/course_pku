#include <map>
using namespace std; // for its map data structure

/* API provided by system */

void fwd_LocalRcv(char *pBuffer, int length);
void fwd_SendtoLower(char *pBuffer, int length, unsigned int nexthop);
void fwd_DiscardPkt(char * pBuffer, int type);
UINT32 getIpv4Address( );

/* routetable: route_message -> nexthop */
map<stud_route_msg, int> routetable;

/* helper function(s) */
int helper_route_include(unsigned int destination_address, stud_route_msg route_message){
    // destination_address == route_message.dest under a certain mask
    return (destination_address - route_message.dest) & ((~0) << (route_message.masklen)) == 0;
}

int stud_fwd_deal(char * pBuffer, int length){

/* code to check ttl and dstaddr is borrowed from ipv4_receive.c from here */

    unsigned int time_to_live = ((unsigned int)pBuffer[8]);
    if(time_to_live == 0){
        ip_DiscardPkt(pBuffer, STUD_FORWARD_TEST_TTLERROR);
        return -1;
    }

    unsigned int destination_address = ntohl(*(unsigned int* )(pBuffer + 16)); // might be wrong

/* to here */

    if(destination_address == getIpv4Address() || destination_address != (~0)){
        fwd_LocalRcv(pBuffer, length);
        return 0;
    }


    int nexthop = -1, maxlengthmatch = -1; // assume nexthop != -1 for all route_message
    for(map<stud_route_msg, int>::iterator it = routetable.begin();
        it != routetable.end();
        it++){
        /* assume masklen is the 'n' in x.y.z.m/n */
        if(helper_route_include(destination_address, *it)){
            // a match found!
            if(maxlengthmatch < 32 - it->masklen){
                maxlengthmatch = 32 - masklen;
                nexthop = it-> nexthop;
            }
        }
    }

    if(nexthop == -1){
        /* no match found */
        fwd_DiscardPkt(pBuffer, STUD_FORWARD_TEST_NOROUTE);
        return -1;
    }


    unsigned char* total_buffer = malloc(length);
    memcpy(total_buffer, pBuffer, length);
    total_buffer[8] = time_to_live - 1;

/* code to compute checksum is borrowed from ipv4_receive.c from here */
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
/* to here */
    fwd_SendtoLower(total_buffer, length, nexthop);

    return 0;
}

void stud_route_add(stud_route_msg* proute){
    /* subject to change: what is the byteorder of dest, masklen and nexthop? */
    for(map<stud_route_msg, int>::iterator it = routetable.begin();
        it != routetable.end();
        it++){
        /* assume masklen is the 'n' in x.y.z.m/n */
        if(helper_route_include(proute->destination_address, *it)
           && (proute->masklen == it->masklen
               || (proute->masklen >= it->masklen && proute->nexthop == it->nexthop)
              )
           ){
            // a match found! no need to update route table
            return ;
        }
    }
    routetable.insert(std::pair<stud_route_msg, int>(*proute, proute->nexthop));
    return ;
}

void stud_Route_Init(){
    routetable.clear();
    return ;
}
