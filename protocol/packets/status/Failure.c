#include <protocol/base.h>
#include "protocol/status/Failure.h"
#include <protocol/CRPPackets.h>
#include <string.h>


CRPPacketFailure *CRPFailureCast(CRPBaseHeader *base)
{
    return (CRPPacketFailure *) base->data;
}

int CRPFailureSend(int sockfd, char *reason)
{

    return CRPSend(CRP_PACKET_FAILURE, reason, strlen(reason), sockfd) != -1;
}