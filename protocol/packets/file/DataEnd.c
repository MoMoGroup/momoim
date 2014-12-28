#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataEnd *) base->data;
}

int CRPFileDataEndSend(int sockfd, uint32_t sessionID, uint8_t code)
{
    return CRPSend(CRP_PACKET_FILE_DATA_END, sessionID, &code, sizeof(code), sockfd) != -1;
}