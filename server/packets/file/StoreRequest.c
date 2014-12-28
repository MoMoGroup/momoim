#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/file.h>

int ProcessPacketFileStoreRequest(OnlineUser *user, uint32_t session, CRPPacketFileStoreRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        if (packet->type == FST_PRIVATE)
        {
            CRPFailureSend(user->sockfd, session, "Private file is not support.");
            return 1;
        }
        if (DataFileExist(packet->key))
        {
            CRPFileDataEndSend(user->sockfd, session, FEC_ALREADY_EXISTS);
        }
        else
        {
        }
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}