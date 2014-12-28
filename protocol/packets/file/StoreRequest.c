#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFileStoreRequest *CRPFileStoreRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFileStoreRequest *) base->data;
}

int CRPFileStoreRequestSend(int sockfd, size_t length, uint8_t type, unsigned char *key)
{
    CRPPacketFileStoreRequest storeRequest = {
            .type=type,
            .length=length
    };
    memcpy(storeRequest.key, key, 16);
    return CRPSend(CRP_PACKET_FILE_STORE_REQUEST, 0, &storeRequest, sizeof(storeRequest), sockfd) != 0;
}