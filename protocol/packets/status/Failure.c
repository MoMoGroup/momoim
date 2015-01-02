#include <protocol/base.h>
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

int CRPFailureSend(int sockfd, uint32_t sessionID, int code, char *reason)
{
    CRPPacketFailure packet = {
            .code=code
    };
    size_t lenReason = strlen(reason);
    memcpy(packet.reason, reason, lenReason);
    return CRPSend(CRP_PACKET_FAILURE, sessionID, reason, (CRP_LENGTH_TYPE) lenReason, sockfd) != -1;
}