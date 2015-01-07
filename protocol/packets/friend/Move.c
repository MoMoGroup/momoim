#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFriendMove *CRPFriendMoveCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendMove *) base->data;
}

int CRPFriendMoveSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t fromGid, uint8_t toGid)
{
    CRPPacketFriendMove packet = {
            .uid=uid,
            .fromGid=fromGid,
            .toGid=toGid
    };
    return CRPSend(context, CRP_PACKET_FRIEND_MOVE, sessionID, &packet, sizeof(CRPPacketFriendMove)) != -1;
}
