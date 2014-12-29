#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>


CRPPacketFriendSearchByNickname *CRPFriendSearchByUsernameCast(CRPBaseHeader *base)
{
    char *mem = (char *) malloc(base->dataLength + 1);
    memcpy(mem, base->data, base->dataLength);
    mem[base->dataLength] = 0;
    return (CRPPacketFriendSearchByNickname *) mem;
}

int CRPFriendSearchByUsernameSend(int sockfd, uint32_t sessionID, const char *text)
{
    return CRPSend(CRP_PACKET_FRIEND_SEARCH_BY_USERNAME, sessionID, text, strlen(text), sockfd) != -1;
}