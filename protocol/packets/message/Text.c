#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>
#include "imcommon/message.h"


CRPPacketMessageText *CRPMessageTextCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageText *) base->data;
}

int CRPMessageTextSend(int sockfd, uint32_t sessionID, USER_MESSAGE_TYPE messageType, uint32_t uid, uint16_t messageLen, char *message)
{
    CRPPacketMessageText *packet = (CRPPacketMessageText *) malloc(sizeof(CRPPacketMessageText) + messageLen);
    packet->messageLen = messageLen;
    packet->uid = uid;
    packet->messageType = messageType;
    memcpy(packet->message, message, messageLen);
    ssize_t ret = CRPSend(CRP_PACKET_MESSAGE_TEXT, sessionID, packet, sizeof(CRPPacketMessageText) + messageLen, sockfd);
    free(packet);
    return ret != -1;
}