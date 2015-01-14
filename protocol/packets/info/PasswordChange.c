#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketInfoPasswordChange *CRPInfoPasswordChangeCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoPasswordChange *) base->data;
}

int CRPInfoPasswordChangeSend(CRPContext context, uint32_t sessionID, const char oldPwd[16], const char newPwd[16])
{
    CRPPacketInfoPasswordChange packet;
    memcpy(packet.oldPwd, oldPwd, 16);
    memcpy(packet.newPwd, newPwd, 16);
    return CRPSend(context,
                   CRP_PACKET_INFO_PASSWORD_CHANGE,
                   sessionID,
                   &packet,
                   sizeof(packet)) != -1;
}
