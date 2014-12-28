#include <protocol/CRPPackets.h>
#include <user.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

int ProcessPacketFriendRequest(OnlineUser *user, uint32_t session, CRPPacketFriendRequest *packet)
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
            CRPFailureSend(user->sockfd, session, "Fail to read user info.");
            return 1;
        }
        void *hey = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (hey == MAP_FAILED)
        {
            close(fd);
            CRPFailureSend(user->sockfd, session, "Fail to mmap file.");
            return 1;
        }
        CRPSend(CRP_PACKET_FRIEND_DATA, 0, hey, buf.st_size, user->sockfd);
        munmap(hey, buf.st_size);
        close(fd);
    }
    else
    {
        CRPFailureSend(user->sockfd, session, "Status Error");
    }
    return 1;
}