#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileData *CRPFileDataCast(CRPBaseHeader *base)
{
    return (CRPPacketFileData *) base->data;
}

int CRPFileDataSend(int sockfd, size_t length, char *data)
{
    return CRPSend(CRP_PACKET_FILE_DATA, 0, data, length, sockfd) != 0;
}