#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <user.h>
#include <data/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/user.h>

int ProcessPacketFileRequest(OnlineUser *user, uint32_t session, CRPPacketFileRequest *packet)
{
    if (packet->type == 0)
    {
        if (DataFileExist(packet->key))
        {
            UserCancelableOperation *operation = NULL;
            char *buf = (char *) malloc(PAGE_SIZE);
            if (buf == NULL)
            {
                CRPFailureSend(user->sockfd, session, "Fail to alloc buffer.");
                return 0;//内存分配失败,注销当前用户
            }
            int fd = DataFileOpen(packet->key, O_RDONLY);
            if (fd < 0)
            {
                CRPFailureSend(user->sockfd, session, "Fail to read file.");
                goto cleanup;
            }
            struct stat fileInfo;
            if (fstat(fd, &fileInfo) == -1)
            {
                CRPFailureSend(user->sockfd, session, "Fail to stat file.");
                goto cleanup;
            }

            operation = UserRegisterOperation(user);
            CRPFileDataStartSend(user->sockfd, session, operation->id, (uint64_t) fileInfo.st_size);
            ssize_t length;
            while (!operation->cancel)
            {
                length = read(fd, buf, PAGE_SIZE);
                if (length == 0)
                {
                    CRPFileDataEndSend(user->sockfd, session, 0);//file end
                    goto cleanup;
                }
                else if (length < 0)
                {
                    CRPFileDataEndSend(user->sockfd, session, 2);//Read unexpected
                    goto cleanup;
                }
                else
                {
                    CRPFileDataSend(user->sockfd, session, length, buf);
                }
            }
            CRPFileDataEndSend(user->sockfd, session, 1);//operation canceled
            cleanup:
            if (operation)
                UserUnregisterOperation(user, operation);
            if (fd >= 0)
                close(fd);
            free(buf);
            return 1;
        }
        else
        {
            CRPFailureSend(user->sockfd, session, "File not found.");
        }
    }
    else
    {
        if (user->status == OUS_ONLINE)
        {
            CRPFailureSend(user->sockfd, session, "Private data file is not support.");
        }
        else
        {
            CRPFailureSend(user->sockfd, session, "Status Error");
        }
    }
    return 1;
}