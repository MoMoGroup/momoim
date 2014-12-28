#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataEnd *) base->data;
}

int CRPFileDataEndSend(int sockfd)
{
    return CRPSend(CRP_PACKET_FILE_DATA_END, 0, NULL, 0, sockfd) != 0;
}