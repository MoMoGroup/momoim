#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFriendGroupAdd *CRPFriendFriendGroupAddCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendGroupAdd *) base->data;
}

int CRPFriendGroupAddSend(CRPContext context, uint32_t sessionID, uint8_t gid, char const name[64])
{
    CRPPacketFriendGroupAdd packet = {
            .gid=gid,
    };
    memcpy(packet.groupName, name, sizeof(packet.groupName));
    return CRPSend(context, CRP_PACKET_FRIEND_GROUP_ADD, sessionID, &packet, sizeof(packet)) != -1;
}
