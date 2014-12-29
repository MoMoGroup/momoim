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

            operation = UserRegisterOperation(user, CUOT_FILE_SEND);
            CRPFileDataStartSend(user->sockfd, session, operation->id, (uint64_t) fileInfo.st_size);
            ssize_t length;
            size_t seq = 0;
            while (!operation->cancel)
            {
                length = read(fd, buf, PAGE_SIZE);
                if (length == 0)
                {
                    CRPFileDataEndSend(user->sockfd, session, FEC_OK);//file end
                    goto cleanup;
                }
                else if (length < 0)
                {
                    CRPFileDataEndSend(user->sockfd, session, FEC_READ_ERROR);//Read unexpected
                    goto cleanup;
                }
                else
                {
                    CRPFileDataSend(user->sockfd, session, length, seq++, buf);
                }
            }
            CRPFileDataEndSend(user->sockfd, session, FEC_CANCELED);//operation canceled
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
            CRPFailureSend(user->sockfd, session, "Private file is not support.");
        }
        else
        {
            CRPFailureSend(user->sockfd, session, "Status Error");
        }
    }
    return 1;
}