#include <string.h>
#include <stdlib.h>
#include <protocol/base.h>
#include "protocol/CRPPackets.h"

CRPPacketFriendAccept *CRPFriendAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendAccept *) base->data;
}

int CRPFriendAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid)
{
    return CRPSend(context, CRP_PACKET_FRIEND_ACCEPT, sessionID, &uid, (CRP_LENGTH_TYPE) sizeof(CRPPacketFriendAccept)) != -1;
}