#include <protocol/CRPPackets.h>

CRPPacketInfoStatusChange *CRPInfoStatusChangeCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoStatusChange *) base->data;
}

int CRPInfoStatusChangeSend(CRPContext context, uint32_t sessionID, uint8_t hidden)
{
    return CRPSend(context, CRP_PACKET_INFO_STATUS_CHANGE, sessionID, &hidden, sizeof(hidden)) != -1;
}
