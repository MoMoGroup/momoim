#include <protocol/base.h>
#include "protocol/status/Failure.h"
#include <protocol/CRPPackets.h>
#include <string.h>


CRPPacketStatusFailure *CRPStatusFailureCast(CRPBaseHeader *base)
{
    return (CRPPacketStatusFailure *) base->data;
}

int CRPStatusFailureSend(int sockfd, char *reason)
{

    return CRPSend(CRP_PACKET_FAILURE, reason, strlen(reason), sockfd) != -1;
}