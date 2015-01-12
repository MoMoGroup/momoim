#include <protocol/CRPPackets.h>
#include <stdlib.h>
#include <string.h>

CRPPacketMessageRecordData *CRPMessageRecordDataCast(CRPBaseHeader *base)
{
    return (CRPPacketMessageRecordData *) base->data;
}

int CRPMessageRecordDataSend(CRPContext context, uint32_t sessionID, uint8_t remain, UserMessage *message)
{
    CRPPacketMessageRecordData *packet = (CRPPacketMessageRecordData *) malloc(sizeof(CRPPacketMessageRecordData) + message->messageLen);
    packet->remain = remain;
    memcpy(&packet->messages, message, sizeof(CRPPacketMessageRecordData));
    memcpy(packet->messages.content, message->content, packet->messages.messageLen);
    return CRPSend(context,
                   CRP_PACKET_MESSAGE_RECORD_DATA,
                   sessionID,
                   packet,
                   (CRP_LENGTH_TYPE) sizeof(CRPPacketMessageRecordData) + message->messageLen) != -1;
}
