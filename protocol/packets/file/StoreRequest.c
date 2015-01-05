#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFileStoreRequest *CRPFileStoreRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFileStoreRequest *) base->data;
}

int CRPFileStoreRequestSend(CRPContext context, uint32_t sessionID, size_t length, uint8_t type, unsigned char *key)
{
    CRPPacketFileStoreRequest storeRequest = {
            .type=type,
            .length=length
    };
    memcpy(storeRequest.key, key, 16);
    return CRPSend(context, CRP_PACKET_FILE_STORE_REQUEST, sessionID, &storeRequest, sizeof(storeRequest)) != -1;
}