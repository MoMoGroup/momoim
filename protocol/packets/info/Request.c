#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketInfoRequest *CRPInfoRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoRequest *) base->data;
}

int CRPInfoRequestSend(CRPContext context, uint32_t sessionID, uint32_t uid)
{
    return CRPSend(context, CRP_PACKET_INFO_REQUEST, sessionID, &uid, sizeof(uid)) != -1;
}
