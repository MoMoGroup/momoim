#include <string.h>
#include <stdlib.h>
#include <protocol/friend/Add.h>
#include "protocol/CRPPackets.h"

CRPPacketFriendAdd *CRPFriendAddCast(CRPBaseHeader *base)
{
    CRPPacketFriendAdd *data = (CRPPacketFriendAdd *) malloc(base->totalLength - sizeof(CRPBaseHeader) + 1);
    memcpy(data, base->data, base->totalLength - sizeof(CRPBaseHeader));
    data->note[base->totalLength - sizeof(CRPBaseHeader) - 4] = 0;
    return data;
}

int CRPFriendAddSend(CRPContext context, uint32_t sessionID, uint32_t uid, const char *note)
{
    int ret;
    size_t lenNote = strlen(note);
    CRPPacketFriendAdd *packet = (CRPPacketFriendAdd *) malloc(sizeof(CRPPacketFriendAdd) + lenNote);
    packet->uid = uid;
    memcpy(packet->note, note, lenNote);
    ret = CRPSend(context,
                  CRP_PACKET_FRIEND_ADD,
                  sessionID,
                  packet,
                  (CRP_LENGTH_TYPE) (sizeof(CRPPacketFriendAdd) + lenNote)) != -1;
    free(packet);
    return ret;
}