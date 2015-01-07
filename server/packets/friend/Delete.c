#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include "run/user.h"

int ProcessPacketFriendDelete(POnlineUser user, uint32_t session, CRPPacketFriendDelete *packet)
{
    if (user->status == OUS_ONLINE)
    {
        UserFriends *friends = user->info->friends;
        UserGroup *group = UserFriendsGroupGet(friends, packet->gid);
        if (!group)
        {
            CRPFailureSend(user->sockfd, session, ENOENT, "分组未找到");
            return 1;
        }
        if (UserFriendsUserDelete(group, packet->uid))
        {
            CRPOKSend(user->sockfd, session);
        }
        else
        {
            CRPFailureSend(user->sockfd, session, EFAULT, "无法删除用户.");
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}