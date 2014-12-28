#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileData *CRPFileDataCast(CRPBaseHeader *base)
{
    return (CRPPacketFileData *) base->data;
}

int CRPFileDataSend(int sockfd, uint32_t sessionID, size_t length, char *data)
{
    return CRPSend(CRP_PACKET_FILE_DATA, sessionID, data, length, sockfd) != 0;
}