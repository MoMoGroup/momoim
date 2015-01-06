#include <protocol/base.h>
#include <protocol/CRPPackets.h>

CRPPacketFileDataEnd *CRPFileDataEndCast(CRPBaseHeader *base)
{
    return (CRPPacketFileDataEnd *) base->data;
}

int CRPFileDataEndSend(CRPContext context, uint32_t sessionID, uint8_t code)
{
    return CRPSend(context, CRP_PACKET_FILE_DATA_END, sessionID, &code, sizeof(code)) != -1;
}