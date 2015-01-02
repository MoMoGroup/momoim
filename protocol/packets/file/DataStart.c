#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFileDataStart *CRPFileDataStartCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataStart *) base->data;
}

int CRPFileDataStartSend(int sockfd, uint32_t sessionID, uint64_t dataLength)
{
    CRPPacketFileDataStart packet = {
            .dataLength=dataLength
    };
    return CRPSend(CRP_PACKET_FILE_DATA_START, sessionID, &packet, sizeof(CRPPacketFileDataStart), sockfd) != -1;

}