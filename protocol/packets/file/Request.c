#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>

CRPPacketFileRequest *CRPFileRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFileRequest *) base->data;
}

int CRPFileRequestSend(int sockfd, uint8_t type, unsigned char *key)
{
    CRPPacketFileRequest request =
            {
                    .type=type
            };
    memcpy(request.key, key, sizeof(request.key));
    return CRPSend(CRP_PACKET_FILE_REQUEST, 0, &request, sizeof(CRPPacketFileRequest), sockfd) != 0;
}
