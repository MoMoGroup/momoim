#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>

CRPPacketKick *CRPKickCast(CRPBaseHeader *base)
{
    CRPPacketKick *data = (CRPPacketKick *) malloc(base->totalLength - sizeof(CRPBaseHeader) + 1);
    memcpy(data, base->data, base->totalLength - sizeof(CRPBaseHeader));
    data->reason[base->totalLength - sizeof(CRPBaseHeader)] = 0;
    return data;
}

int CRPKickSend(CRPContext context, uint32_t sessionID, const char *reason)
{
    return CRPSend(context, CRP_PACKET_KICK, sessionID, reason, (CRP_LENGTH_TYPE) strlen(reason)) != -1;
}