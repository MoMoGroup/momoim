#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFileReset *CRPFileResetCast(CRPBaseHeader *base)
{
    return (CRPPacketFileReset *) base->data;
}

int CRPFileResetSend(int sockfd, uint32_t sessionID, size_t seq)
{
    return CRPSend(CRP_PACKET_FILE_RESET, sessionID, &seq, sizeof(seq), sockfd) != -1;
}