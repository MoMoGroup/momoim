#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>


CRPPacketInfoData *CRPInfoDataCast(CRPBaseHeader *base)
{
    return (CRPPacketInfoData *) base->data;
}

int CRPInfoDataSend(int sockfd, uint32_t sessionID, UserInfo *info)
{
    return CRPSend(CRP_PACKET_INFO_DATA, sessionID, info, sizeof(UserInfo), sockfd) != -1;
}