#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFileRequest *CRPFileRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFileRequest *) base->data;
}

int CRPFileRequestSend(CRPContext context, uint32_t sessionID, uint8_t type, unsigned char *key)
{
    CRPPacketFileRequest request =
            {
                    .type=type
            };
    memcpy(request.key, key, sizeof(request.key));
    return CRPSend(context, CRP_PACKET_FILE_REQUEST, sessionID, &request, sizeof(CRPPacketFileRequest)) != -1;
}
