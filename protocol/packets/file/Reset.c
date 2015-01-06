#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFileReset *CRPFileResetCast(CRPBaseHeader *base)
{
    return (CRPPacketFileReset *) base->data;
}

int CRPFileResetSend(CRPContext context, uint32_t sessionID, size_t seq)
{
    return CRPSend(context, CRP_PACKET_FILE_RESET, sessionID, &seq, sizeof(seq)) != -1;
}