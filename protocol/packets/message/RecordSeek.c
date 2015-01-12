#include <protocol/CRPPackets.h>

CRPPacketMessageRecordSeek *CRPMessageRecordSeekCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageRecordSeek *) base->data;
}

int CRPMessageRecordSeekSend(CRPContext context, uint32_t sessionID, uint32_t date)
{
    return CRPSend(context,
                   CRP_PACKET_MESSAGE_RECORD_SEEK,
                   sessionID,
                   &date,
                   sizeof(date)) != -1;
}
