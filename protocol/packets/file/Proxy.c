#include <protocol/CRPPackets.h>

CRPPacketFileProxyRequest *CRPFileProxyRequestCast(CRPBaseHeader *base)
{
    return (CRPPacketFileProxyRequest *) base->data;
}

int CRPFileProxyRequestSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint64_t bandwidthRequest)
{
    CRPPacketFileProxyRequest packet = {
            .uid=uid,
            .fileSize=bandwidthRequest
    };
    return CRPSend(context,
                   CRP_PACKET_FILE_PROXY,
                   sessionID,
                   &packet,
                   sizeof(CRPPacketFileProxyRequest)
                  ) != -1;
}
