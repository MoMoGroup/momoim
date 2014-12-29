#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketFileStoreAccept *CRPFileStoreAcceptCast(CRPBaseHeader *base)
{
    return (CRPPacketFileStoreAccept *) base->data;
}

int CRPFileStoreAcceptSend(int sockfd, uint32_t sessionID, uint32_t operationId)
{
    return CRPSend(CRP_PACKET_FILE_STORE_ACCEPT, sessionID, &operationId, sizeof(uint32_t), sockfd) != -1;
}