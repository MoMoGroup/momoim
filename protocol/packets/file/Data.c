#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>

CRPPacketFileData *CRPFileDataCast(CRPBaseHeader *base)
{
    CRPPacketFileData *packet = (CRPPacketFileData *) malloc(base->dataLength + sizeof(NILOBJ(CRPPacketFileData)->length));
    packet->length = base->dataLength - sizeof(NILOBJ(CRPPacketFileData)->seq);
    memcpy(&packet->seq, base->data, sizeof(packet->seq));
    memcpy(packet->data, base->data + sizeof(packet->seq), packet->length);
    return packet;
}

int CRPFileDataSend(int sockfd, uint32_t sessionID, CRP_LENGTH_TYPE length, size_t seq, char *data)
{
    char *mem = (char *) malloc(length + sizeof(seq));
    memcpy(mem, &seq, sizeof(seq));
    memcpy(mem + sizeof(seq), data, length);
    int ret = CRPSend(CRP_PACKET_FILE_DATA, sessionID, mem, length + sizeof(seq), sockfd) != -1;
    free(mem);
    return ret;
}