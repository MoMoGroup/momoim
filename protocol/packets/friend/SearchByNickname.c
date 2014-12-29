#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>


CRPPacketFriendSearchByNickname *CRPFriendSearchByNicknameCast(CRPBaseHeader *base)
{
    char *mem = (char *) malloc(base->dataLength + 1);
    memcpy(mem, base->data, base->dataLength);
    mem[base->dataLength] = 0;
    return (CRPPacketFriendSearchByNickname *) mem;
}

int CRPFriendSearchByNicknameSend(int sockfd, uint32_t sessionID, const char *text)
{
    return CRPSend(CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME, sessionID, text, strlen(text), sockfd) != -1;
}