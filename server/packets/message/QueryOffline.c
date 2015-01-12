#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <datafile/message.h>
#include <datafile/user.h>
#include <stdlib.h>
#include "run/user.h"

int ProcessPacketMessageQueryOffline(POnlineUser user, uint32_t session, CRPPacketMessageQueryOffline *packet)
{
    if (user->status == OUS_ONLINE) {
        MessageFile *file = UserMessageFileGet(user->info->uid);
        UserInfo *info = UserInfoGet(user->info->uid);
        if (MessageFileSeek(file, (uint32_t) (info->lastlogout / (24 * 60 * 60)))) {
            CRPOKSend(user->sockfd, session);
        }
        else {
            CRPFailureSend(user->sockfd, session, EFAULT, "无法找到下线时间.");
        }
        UserInfoFree(info);
        UserMessage *msg;
        while ((msg = MessageFileNext(file)) != NULL) {
            if (msg->time >= info->lastlogout) {
                if (!CRPMessageNormalSend(user->sockfd, 0, msg->messageType, msg->from, msg->messageLen, msg->content)) {
                    break;
                }
            }
            free(msg);
        }
        UserMessageFileDrop(user->info->uid);

    }
    else {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}