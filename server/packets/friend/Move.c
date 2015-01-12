#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendMove(POnlineUser user, uint32_t session, CRPPacketFriendMove *packet)
{
    if (user->status == OUS_ONLINE) {
        UserFriends *friends = user->info->friends;
        UserGroup *groupFrom = UserFriendsGroupGet(friends, packet->fromGid),
                *groupTo = UserFriendsGroupGet(friends, packet->toGid);
        if (!groupFrom || !groupTo) {
            CRPFailureSend(user->sockfd, session, ENOENT, "分组未找到");
            return 1;
        }

        if (!UserFriendsUserAdd(groupTo, packet->uid)) {
            CRPFailureSend(user->sockfd, session, ENOMEM, "无法添加用户到目标分组");
            return 1;
        }
        UserFriendsUserDelete(groupFrom, packet->uid);
        CRPOKSend(user->sockfd, session);
    }
    else {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}