#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>


CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoData *) base->data;
}

int CRPInfoDataSend(int sockfd, uint32_t sessionID, uint32_t uid, char *nick, char sex, unsigned char *icon)
{
    CRPPacketInfoData data = {
            .uid=uid,
            .sex=sex,
    };
    memcpy(data.nickName, nick, sizeof(data.nickName));
    memcpy(data.icon, icon, sizeof(data.icon));
    return CRPSend(CRP_PACKET_INFO_DATA, sessionID, &data, sizeof(data), sockfd) != -1;
}