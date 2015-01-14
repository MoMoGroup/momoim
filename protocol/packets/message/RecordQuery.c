#include <protocol/CRPPackets.h>

CRPPacketMessageRecordQuery *CRPMessageRecordQueryCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageRecordQuery *) base->data;
}

int CRPMessageRecordQuerySend(CRPContext context, uint32_t sessionID, CRPPacketMessageRecordQuery *condition)
{
    return CRPSend(context,
                   CRP_PACKET_MESSAGE_RECORD_QUERY,
                   sessionID,
                   condition,
                   sizeof(CRPPacketMessageRecordQuery)) != -1;
}
