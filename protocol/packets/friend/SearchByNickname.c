#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>


CRPPacketFriendSearchByNickname *CRPFriendSearchByNicknameCast(CRPBaseHeader *base)
{
    char *mem = (char *) malloc(base->totalLength - sizeof(CRPBaseHeader) + 1);
    memcpy(mem, base->data, base->totalLength - sizeof(CRPBaseHeader));
    mem[base->totalLength - sizeof(CRPBaseHeader)] = 0;
    return (CRPPacketFriendSearchByNickname *) mem;
}

int CRPFriendSearchByNicknameSend(CRPContext context, uint32_t sessionID, uint8_t page, uint8_t size, const char *text)
{
    CRPPacketFriendSearchByNickname *packet =
            (CRPPacketFriendSearchByNickname *) malloc(sizeof(CRPPacketFriendSearchByNickname) + strlen(text));
    int ret = CRPSend(
            context,
            CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME,
            sessionID,
            packet,
            (CRP_LENGTH_TYPE) (sizeof(CRPPacketFriendSearchByNickname) + strlen(text))
    ) != -1;
    free(packet);
    return ret;
}