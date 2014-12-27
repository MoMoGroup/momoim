#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>


CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoData *) base->data;
}

int CRPInfoDataSend(int sockfd, uint32_t uid, char *nick, char sex)
{
    CRPPacketInfoData data = {
            .uid=uid,
            .sex=sex
    };
    memcpy(data.nickName, nick, sizeof(data.nickName));
    return CRPSend(CRP_PACKET_INFO_DATA, &data, sizeof(data), sockfd) != 0;
}