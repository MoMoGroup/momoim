#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>

CRPPacketFriendUserList *CRPFriendUserListCast(CRPBaseHeader *base)
{
    CRPPacketFriendUserList *packet = (CRPPacketFriendUserList *) malloc(sizeof(uint16_t) + base->dataLength);
    packet->count = (uint16_t) (base->dataLength / sizeof(uint32_t));
    memcpy(packet->users, base->data, base->dataLength);
    return packet;
}

int CRPFriendUserListSend(int sockfd, uint32_t sessionID, uint32_t *uids, uint16_t count)
{
    return CRPSend(CRP_PACKET_FRIEND_USER_LIST, sessionID, uids, (CRP_LENGTH_TYPE) (count * sizeof(uint32_t)), sockfd) != -1;
}