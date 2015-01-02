#include <protocol/base.h>
#include <protocol/CRPPackets.h>


CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoData *) base->data;
}

int CRPInfoDataSend(int sockfd, uint32_t sessionID, int isOnline, UserInfo *info)
{
    CRPPacketInfoData data = {
            .isOnline=isOnline,
            .info=*info
    };
    return CRPSend(CRP_PACKET_INFO_DATA, sessionID, &data, sizeof(CRPPacketInfoData), sockfd) != -1;
}