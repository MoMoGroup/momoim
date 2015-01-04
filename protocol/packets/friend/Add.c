#include <string.h>
#include <stdlib.h>
#include <protocol/friend/Add.h>
#include "protocol/CRPPackets.h"

CRPPacketFriendAdd *CRPFriendAddCast(CRPBaseHeader *base)
{
    CRPPacketFriendAdd *data = (CRPPacketFriendAdd *) malloc(base->dataLength + 1);
    memcpy(data, base->data, base->dataLength);
    data->note[base->dataLength] = 0;
    return data;
}

int CRPFriendAddSend(int sockfd, uint32_t sessionID, uint32_t uid, const char *note)
{

    CRPPacketFriendAdd packet = {
            .uid=uid
    };
    size_t lenNote = strlen(note);
    memcpy(packet.note, note, lenNote);
    return CRPSend(CRP_PACKET_FAILURE, sessionID, &packet, (CRP_LENGTH_TYPE) (sizeof(CRPPacketFriendData) + lenNote), sockfd) != -1;
}