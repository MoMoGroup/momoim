#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "fcntl.h"
#include <protocol/base.h>
#include <protocol/CRPPackets.h>

void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *base) = {
        [CRP_PACKET_KEEP_ALIVE]         = (void *(*)(CRPBaseHeader *base)) CRPKeepAliveCast,
        [CRP_PACKET_HELLO]              = (void *(*)(CRPBaseHeader *base)) CRPHelloCast,
        [CRP_PACKET_OK]                 = (void *(*)(CRPBaseHeader *base)) CRPOKCast,
        [CRP_PACKET_FAILURE]            = (void *(*)(CRPBaseHeader *base)) CRPFailureCast,
        [CRP_PACKET_CRASH]              = (void *(*)(CRPBaseHeader *base)) CRPCrashCast,

        [CRP_PACKET_LOGIN__START]       = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        = (void *(*)(CRPBaseHeader *base)) CRPLoginLoginCast,
        [CRP_PACKET_LOGIN_LOGOUT]       = (void *(*)(CRPBaseHeader *base)) CRPLoginLogoutCast,
        [CRP_PACKET_LOGIN_REGISTER]     = (void *(*)(CRPBaseHeader *base)) CRPLoginRegisterCast,

        [CRP_PACKET_INFO__START]        = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_INFO_REQUEST]       = (void *(*)(CRPBaseHeader *base)) CRPInfoRequestCast,
        [CRP_PACKET_INFO_DATA]          = (void *(*)(CRPBaseHeader *base)) CRPInfoDataCast,

        [CRP_PACKET_FRIEND__START]      = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_FRIEND_REQUEST]     = (void *(*)(CRPBaseHeader *base)) CRPFriendRequestCast,
        [CRP_PACKET_FRIEND_DATA]        = (void *(*)(CRPBaseHeader *base)) CRPFriendDataCast,

        [CRP_PACKET_FILE__START]        = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_FILE_REQUEST]       = (void *(*)(CRPBaseHeader *base)) CRPFileRequestCast,
        [CRP_PACKET_FILE_DATA]          = (void *(*)(CRPBaseHeader *base)) CRPFileDataCast,
        [CRP_PACKET_FILE_DATA_END]      = (void *(*)(CRPBaseHeader *base)) CRPFileDataEndCast,
        [CRP_PACKET_FILE_STORE_REQUEST] = (void *(*)(CRPBaseHeader *base)) CRPFileStoreRequestCast,

        [CRP_PACKET_MESSAGE__START]     = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       = (void *(*)(CRPBaseHeader *base)) CRPMessageTextCast,
};

ssize_t CRPSend(uint16_t packetID, uint32_t sessionID, void const *data, size_t length, int fd)
{
    CRPBaseHeader *header = (CRPBaseHeader *) malloc(sizeof(CRPBaseHeader) + length);
    header->magicCode = 0x464F5573;
    header->totalLength = (CRP_LENGTH_TYPE) (sizeof(CRPBaseHeader) + length);
    header->dataLength = (CRP_LENGTH_TYPE) length;
    header->packetID = packetID;
    header->sessionID = sessionID;
    if (length)
        memcpy(header->data, data, length);
    ssize_t len = send(fd, header, header->totalLength, 0);
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
