#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>

CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base)
{
    CRPPacketFailure *data = (CRPPacketFailure *) malloc(base->totalLength - sizeof(CRPBaseHeader) + 1);
    memcpy(data, base->data, base->totalLength - sizeof(CRPBaseHeader));
    data->reason[base->totalLength - sizeof(CRPBaseHeader)] = 0;
    return data;
}

int CRPFailureSend(int sockfd, uint32_t sessionID, uint8_t code, char *reason)
{
    size_t lenReason = strlen(reason);
    CRPPacketFailure *packet = (CRPPacketFailure *) malloc(sizeof(CRPPacketFailure) + lenReason);
    memcpy(&packet->code, &code, sizeof(uint8_t));
    memcpy(packet->reason, reason, lenReason);
    return CRPSend(CRP_PACKET_FAILURE, sessionID, packet, (CRP_LENGTH_TYPE) (sizeof(CRPPacketFailure) + lenReason), sockfd) != -1;
}