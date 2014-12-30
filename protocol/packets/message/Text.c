#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>


CRPPacketMessageText *CRPMessageTextCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageText *) base->data;
}

int CRPMessageTextSend(int sockfd, uint32_t sessionID, uint32_t userid, uint16_t message_len, char *message)
{
    CRPPacketMessageText *packet = (CRPPacketMessageText *) malloc(sizeof(CRPPacketMessageText) + message_len);
    packet->message_len = message_len;
    packet->touid = userid;
    memcpy(packet->message, message, message_len);
    ssize_t ret = CRPSend(CRP_PACKET_MESSAGE_TEXT, sessionID, packet, sizeof(CRPPacketMessageText) + message_len, sockfd);
    free(packet);
    return ret != -1;
}