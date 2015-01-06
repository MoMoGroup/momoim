#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFileDataStart *CRPFileDataStartCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataStart *) base->data;
}

int CRPFileDataStartSend(CRPContext context, uint32_t sessionID, uint64_t dataLength)
{
    CRPPacketFileDataStart packet = {
            .dataLength=dataLength
    };
    return CRPSend(context, CRP_PACKET_FILE_DATA_START, sessionID, &packet, sizeof(CRPPacketFileDataStart)) != -1;

}