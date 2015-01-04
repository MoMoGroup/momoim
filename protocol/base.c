#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "fcntl.h"
#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <errno.h>
#include <stdio.h>

void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *base) = {
        [CRP_PACKET_KEEP_ALIVE]         = (void *(*)(CRPBaseHeader *base)) CRPKeepAliveCast,
        [CRP_PACKET_HELLO]              = (void *(*)(CRPBaseHeader *base)) CRPHelloCast,
        [CRP_PACKET_OK]                 = (void *(*)(CRPBaseHeader *base)) CRPOKCast,
        [CRP_PACKET_FAILURE]            = (void *(*)(CRPBaseHeader *base)) CRPFailureCast,
        [CRP_PACKET_CRASH]              = (void *(*)(CRPBaseHeader *base)) CRPCrashCast,
        [CRP_PACKET_KICK]               = (void *(*)(CRPBaseHeader *base)) CRPKickCast,
        [CRP_PACKET_CANCEL]             = (void *(*)(CRPBaseHeader *base)) CRPCancelCast,

        [CRP_PACKET_LOGIN__START]       = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_LOGIN_ACCEPT]       = (void *(*)(CRPBaseHeader *base)) CRPLoginAcceptCast,
        [CRP_PACKET_LOGIN_LOGIN]        = (void *(*)(CRPBaseHeader *base)) CRPLoginLoginCast,
        [CRP_PACKET_LOGIN_LOGOUT]       = (void *(*)(CRPBaseHeader *base)) CRPLoginLogoutCast,
        [CRP_PACKET_LOGIN_REGISTER]     = (void *(*)(CRPBaseHeader *base)) CRPLoginRegisterCast,

        [CRP_PACKET_INFO__START]        = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_INFO_REQUEST]       = (void *(*)(CRPBaseHeader *base)) CRPInfoRequestCast,
        [CRP_PACKET_INFO_DATA]          = (void *(*)(CRPBaseHeader *base)) CRPInfoDataCast,

        [CRP_PACKET_FRIEND__START]      = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (void *(*)(CRPBaseHeader *base)) CRPFriendRequestCast,
        [CRP_PACKET_FRIEND_DATA]        = (void *(*)(CRPBaseHeader *base)) CRPFriendDataCast,
        [CRP_PACKET_FRIEND_ADD]         = (void *(*)(CRPBaseHeader *base)) CRPFriendAddCast,
        [CRP_PACKET_FRIEND_SEARCH_BY_NICKNAME]= (void *(*)(CRPBaseHeader *base)) CRPFriendSearchByNicknameCast,
        [CRP_PACKET_FRIEND_USER_LIST]   = (void *(*)(CRPBaseHeader *base)) CRPFriendUserListCast,

        [CRP_PACKET_FILE__START]        = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_FILE_DATA]          = (void *(*)(CRPBaseHeader *base)) CRPFileDataCast,
        [CRP_PACKET_FILE_DATA_END]      = (void *(*)(CRPBaseHeader *base)) CRPFileDataEndCast,
        [CRP_PACKET_FILE_DATA_START]    = (void *(*)(CRPBaseHeader *base)) CRPFileDataStartCast,
        [CRP_PACKET_FILE_REQUEST]       = (void *(*)(CRPBaseHeader *base)) CRPFileRequestCast,
        [CRP_PACKET_FILE_RESET]         = (void *(*)(CRPBaseHeader *base)) CRPFileResetCast,
        [CRP_PACKET_FILE_STORE_REQUEST] = (void *(*)(CRPBaseHeader *base)) CRPFileStoreRequestCast,

        [CRP_PACKET_MESSAGE__START]     = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_MESSAGE_NORMAL]       = (void *(*)(CRPBaseHeader *base)) CRPMessageNormalCast,
};

ssize_t CRPSend(packet_id_t packetID, session_id_t sessionID, void const *data, CRP_LENGTH_TYPE length, int fd)
{
    CRPBaseHeader *header = (CRPBaseHeader *) malloc(sizeof(CRPBaseHeader) + length);
    header->magicCode = 0x464F5573;
    header->totalLength = (CRP_LENGTH_TYPE) (sizeof(CRPBaseHeader) + length);
    //header->dataLength = length;
    header->packetID = packetID;
    header->sessionID = sessionID;
    if (length)
        memcpy(header->data, data, length);
    ssize_t len;
    while (-1 == (len = send(fd, header, header->totalLength, 0)) && (errno == EWOULDBLOCK || errno == EAGAIN))
    {
        //如果系统要求重发则阻塞当前线程,等待重发
        fd_set fdWr, fdEx;
        FD_ZERO(&fdWr);
        FD_SET(fd, &fdWr);
        fdEx = fdWr;
        struct timeval time = {
                .tv_sec=3
        };
        int n = select(fd + 1, NULL, &fdWr, &fdEx, &time);
        if (n == -1 || FD_ISSET(fd, &fdEx))//如果select失败或者fd异常.警告用户.
        {
            perror("Warning: Fail to send packet");
            free(header);
            return -1;
        }
        //否则任何情况都重试
    }
    free(header);
    return len;
}

CRPBaseHeader *CRPRecv(int fd)
{
    CRPBaseHeader h;
    ssize_t ret;

    ret = recv(fd, &h, sizeof(CRPBaseHeader), MSG_PEEK);
    if (ret != sizeof(CRPBaseHeader) || h.magicCode != 0x464F5573)
    {
        return NULL;
    }
    CRPBaseHeader *packet = (CRPBaseHeader *) malloc(h.totalLength);
    ret = recv(fd, packet, h.totalLength, MSG_WAITALL);
    if (ret != h.totalLength)
    {
        //log_warning("DATA", "Recv Error 2-%d bytes.%d/%s\n", ret, errno, strerror(errno));
        free(packet);
        return NULL;
    }
    return packet;
}

CRPBaseHeader *CRPRecvNonBlock(int fd)
{
    CRPBaseHeader h;
    ssize_t ret;

    ret = recv(fd, &h, sizeof(CRPBaseHeader), MSG_PEEK | O_NONBLOCK);
    if (ret != sizeof(CRPBaseHeader) || h.magicCode != 0x464F5573)
    {
        return NULL;
    }
    CRPBaseHeader *packet = (CRPBaseHeader *) malloc(h.totalLength);
    ret = recv(fd, packet, h.totalLength, MSG_WAITALL);
    if (ret != h.totalLength)
    {
        free(packet);
        return NULL;
    }
    return packet;
}
