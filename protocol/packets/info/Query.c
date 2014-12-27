#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketInfoQuery *CRPInfoQueryCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoQuery *) base->data;
}

int CRPInfoQuerySend(int sockfd, uint32_t uid)
{
    return CRPSend(CRP_PACKET_INFO_QUERY, &uid, sizeof(uid), sockfd) != 0;
}