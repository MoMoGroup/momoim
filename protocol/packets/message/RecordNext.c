#include <protocol/CRPPackets.h>

CRPPacketMessageRecordNext *CRPMessageRecordNextCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageRecordNext *) base->data;
}

int CRPMessageRecordNextSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint8_t size)
{
    CRPPacketMessageRecordNext packet = {
            .uid=uid,
            .size=size
    };
    return CRPSend(context,
                   CRP_PACKET_MESSAGE_RECORD_NEXT,
                   sessionID,
                   &packet,
                   sizeof(packet)) != -1;
}
