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

int CRPFriendSearchByNicknameSend(int sockfd, uint32_t sessionID, uint8_t page, uint8_t size, const char *text)
{
    CRPPacketFriendSearchByNickname *packet =
            (CRPPacketFriendSearchByNickname *) malloc(sizeof(CRPPacketFriendSearchByNickname) + strlen(text));
    int ret = CRPSend(
            CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME,
            sessionID,
            packet,
            (CRP_LENGTH_TYPE) (sizeof(CRPPacketFriendSearchByNickname) + strlen(text)),
            sockfd) != -1;
    free(packet);
    return ret;
}