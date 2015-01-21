#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>


CRPPacketMessageNormal *CRPMessageNormalCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageNormal *) base->data;
}

int CRPMessageNormalSend(CRPContext context, uint32_t sessionID, USER_MESSAGE_TYPE messageType, uint32_t uid,
                         time_t time, uint16_t messageLen, char *message)
{
    CRPPacketMessageNormal *packet = (CRPPacketMessageNormal *) malloc(sizeof(CRPPacketMessageNormal) + messageLen);
    packet->messageLen = messageLen;
    packet->uid = uid;
    packet->messageType = messageType;
    packet->time = time;
    memcpy(packet->message, message, messageLen);
    ssize_t ret = CRPSend(context,
                          CRP_PACKET_MESSAGE_NORMAL,
                          sessionID,
                          packet,
                          sizeof(CRPPacketMessageNormal) + messageLen);
    free(packet);
    return ret != -1;
}