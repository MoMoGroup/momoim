#include <protocol/base.h>
#include "protocol/status/Failure.h"
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>

CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base)
{
    CRPPacketFailure *data = (CRPPacketFailure *) malloc(base->dataLength + 1);
    memcpy(data, base->data, base->dataLength);
    data->reason[base->dataLength] = 0;
    return data;
}

int CRPFailureSend(int sockfd, uint32_t sessionID, char *reason)
{

    return CRPSend(CRP_PACKET_FAILURE, sessionID, reason, strlen(reason), sockfd) != -1;
}