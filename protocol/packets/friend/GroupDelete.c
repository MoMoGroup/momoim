#include <protocol/friend/GroupDelete.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendGroupDelete *CRPFriendFriendGroupDeleteCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendGroupDelete *) base->data;
}

int CRPFriendGroupDeleteSend(CRPContext context, uint32_t sessionID, uint8_t gid)
{
    return CRPSend(context, CRP_PACKET_FRIEND_GROUP_DELETE, sessionID, &gid, sizeof(gid)) != -1;
}
