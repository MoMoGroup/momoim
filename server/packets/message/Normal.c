#include <protocol/CRPPackets.h>
#include <data/user.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
#include "run/user.h"

int ProcessPacketMessageText(OnlineUser *user, uint32_t session, CRPPacketMessageNormal *packet)
{
    if (user->status == OUS_ONLINE)
    {
        OnlineUser *toUser = OnlineUserGet(packet->uid);
        if (toUser == NULL)
        {
            MessageFile *file = UserMessageFileOpen(packet->uid);
            UserMessage *msg = (UserMessage *) malloc(sizeof(UserMessage) + packet->messageLen);
            UserMessage t = {
                    .messageType=packet->messageType,
                    .messageLen=packet->messageLen,
                    .from=user->info->uid,
                    .to=packet->uid
            };
            memcpy(msg, &t, sizeof(UserMessage));
            memcpy(msg->content, packet->message, packet->messageLen);
            log_info("MSG", "离线消息以保存\n");
            if (MessageFileAppend(file, msg))
            {
                CRPOKSend(user->sockfd, session);
            }
            else
            {
                CRPFailureSend(user->sockfd, session, "Message Failure");
            }
        }
        else
        {
            log_info("MSG", "消息转发由%u到%u\n", user->info->uid, packet->uid);
            CRPMessageNormalSend(toUser->sockfd, 0, (uint8_t) packet->messageType, user->info->uid, packet->messageLen, packet->message);
            OnlineUserUnhold(toUser);
            CRPOKSend(user->sockfd, session);
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}