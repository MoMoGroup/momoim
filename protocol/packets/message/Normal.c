#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>
#include "imcommon/message.h"


CRPPacketMessageNormal *CRPMessageNormalCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageNormal *) base->data;
}

int CRPMessageNormalSend(int sockfd, uint32_t sessionID, USER_MESSAGE_TYPE messageType, uint32_t uid, uint16_t messageLen, char *message)
{
    CRPPacketMessageNormal *packet = (CRPPacketMessageNormal *) malloc(sizeof(CRPPacketMessageNormal) + messageLen);
    packet->messageLen = messageLen;
    packet->uid = uid;
    packet->messageType = messageType;
    memcpy(packet->message, message, messageLen);
    ssize_t ret = CRPSend(CRP_PACKET_MESSAGE_NORMAL, sessionID, packet, sizeof(CRPPacketMessageNormal) + messageLen, sockfd);
    free(packet);
    return ret != -1;
}