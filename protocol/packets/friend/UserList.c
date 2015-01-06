#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>

CRPPacketFriendUserList *CRPFriendUserListCast(CRPBaseHeader *base)
{
    CRPPacketFriendUserList *packet = (CRPPacketFriendUserList *) malloc(sizeof(uint16_t) + base->totalLength - sizeof(CRPBaseHeader));
    packet->count = (uint16_t) ((base->totalLength - sizeof(CRPBaseHeader)) / sizeof(uint32_t));
    memcpy(packet->users, base->data, base->totalLength - sizeof(CRPBaseHeader));
    return packet;
}

int CRPFriendUserListSend(CRPContext context, uint32_t sessionID, uint32_t *uids, uint16_t count)
{
    return CRPSend(context, CRP_PACKET_FRIEND_USER_LIST, sessionID, uids, (CRP_LENGTH_TYPE) (count * sizeof(uint32_t))) != -1;
}