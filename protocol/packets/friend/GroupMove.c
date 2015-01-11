#include <protocol/friend/GroupMove.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendGroupMove *CRPFriendFriendGroupMoveCast(CRPBaseHeader *base)
{

    return (CRPPacketFriendGroupMove *) base->data;
}

int CRPFriendGroupMoveSend(CRPContext context, uint32_t sessionID, uint8_t gid, uint8_t nextGid)
{
    CRPPacketFriendGroupMove packet = {
            .gid=gid,
            .nextGid=nextGid
    };
    return CRPSend(context, CRP_PACKET_FRIEND_GROUP_MOVE, sessionID, &packet, sizeof(CRPPacketFriendGroupMove)) != -1;
}
