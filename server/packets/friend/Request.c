#include <protocol/CRPPackets.h>
#include <user.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int ProcessPacketFriendRequest(OnlineUser *user, CRPPacketFriendRequest *packet)
{
    if (user->status == OUS_ONLINE)
    {
        char path[50];
        sprintf(path, "%s%s", user->info->userDir, "friends");
        struct stat buf;
        int fd = open(path, O_RDONLY);
        if (fstat(fd, &buf))
        {
            close(fd);
            CRPFailureSend(user->sockfd, "Fail to get user info.");

        }

    }
    else
    {
        CRPFailureSend(user->sockfd, "Status Error");
    }
    return 1;
}