#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>

CRPPacketFileData *CRPFileDataCast(CRPBaseHeader *base)
{
    CRPPacketFileData *packet = (CRPPacketFileData *) malloc(sizeof(CRPPacketFileData) + base->dataLength);
    packet->length = base->dataLength;
    return packet;
}

int CRPFileDataSend(int sockfd, uint32_t sessionID, size_t length, char *data)
{
    return CRPSend(CRP_PACKET_FILE_DATA, sessionID, data, length, sockfd) != -1;
}