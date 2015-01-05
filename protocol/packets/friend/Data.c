#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <stdlib.h>

CRPPacketFriendData *CRPFriendDataCast(CRPBaseHeader *base)
{
    return (CRPPacketFriendData *) base->data;
}

int CRPFriendDataSend(CRPContext context, uint32_t sessionID, UserFriends *friends)
{
    size_t size = UserFriendsSize(friends);
    void *data = malloc(size);
    UserFriendsEncode(friends, data);

    ssize_t ret = CRPSend(context, CRP_PACKET_FRIEND_DATA, sessionID, data, size);
    free(data);

    return ret != -1;
}