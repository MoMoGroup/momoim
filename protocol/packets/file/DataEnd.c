#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataEnd *) base->data;
}

int CRPFileDataEndSend(int sockfd, uint32_t sessionID)
{
    return CRPSend(CRP_PACKET_FILE_DATA_END, sessionID, NULL, 0, sockfd) != -1;
}