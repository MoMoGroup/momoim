#include <protocol/CRPPackets.h>
#include <string.h>
#include <data/user.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendSearchByNickname(POnlineUser user, uint32_t session, CRPPacketFriendSearchByNickname *packet)
{
    if (user->status != OUS_ONLINE)
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    else
    {
        size_t textLength = strlen(packet->text);
        char s[textLength + 3];//%<text>%\0
        uint32_t users[packet->size];
        memcpy(s + 1, packet->text, textLength);
        s[0] = '%';
        s[textLength + 1] = '%';
        s[textLength + 2] = 0;
        int count = UserQueryByNick(s, packet->page, packet->size, users);
        CRPFriendUserListSend(user->sockfd, session, users, count);
    }
    return 1;
}