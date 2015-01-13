#include <protocol/CRPPackets.h>

CRPPacketInfoPasswordChange *CRPInfoPasswordChangeCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoPasswordChange *) base->data;
}

int CRPInfoPasswordChangeSend(CRPContext context, uint32_t sessionID, char const password[16])
{
    return CRPSend(context,
                   CRP_PACKET_INFO_PASSWORD_CHANGE,
                   sessionID,
                   password,
                   16) != -1;
}
