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

int CRPKickSend(int sockfd, uint32_t sessionID, const char *reason)
{
    return CRPSend(CRP_PACKET_KICK, sessionID, reason, (CRP_LENGTH_TYPE) strlen(reason), sockfd) != -1;
}