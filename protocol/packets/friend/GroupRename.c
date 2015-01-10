#include <protocol/friend/GroupRename.h>
#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFriendGroupRename *CRPFriendFriendGroupRenameCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendGroupRename *) base->data;
}

int CRPFriendGroupRenameSend(CRPContext context, uint32_t sessionID, uint8_t gid, char const name[64])
{
    CRPPacketFriendGroupRename packet = {
            .gid=gid
    };
    memcpy(packet.groupName, name, sizeof(packet.groupName));
    return CRPSend(context, CRP_PACKET_FRIEND_GROUP_RENAME, sessionID, &packet, sizeof(CRPPacketFriendGroupRename)) != -1;
}
