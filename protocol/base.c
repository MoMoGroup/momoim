#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>

#include <protocol/base.h>
#include <protocol/CRPPackets.h>

void *(*const PacketsDataCastMap[CRP_PACKET_ID_MAX + 1])(CRPBaseHeader *base) = {
        [CRP_PACKET_KEEP_ALIVE]         = (void *(*)(CRPBaseHeader *base)) CRPKeepAliveCast,
        [CRP_PACKET_HELLO]              = (void *(*)(CRPBaseHeader *base)) CRPHelloCast,
        [CRP_PACKET_OK]                 = (void *(*)(CRPBaseHeader *base)) CRPOKCast,
        [CRP_PACKET_CRASH]              = (void *(*)(CRPBaseHeader *base)) CRPCrashCast,

        [CRP_PACKET_LOGIN__START]       = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_LOGIN_LOGIN]        = (void *(*)(CRPBaseHeader *base)) CRPLoginLoginCast,
        [CRP_PACKET_FAILURE]      = (void *(*)(CRPBaseHeader *base)) CRPFailureCast,
        [CRP_PACKET_LOGIN_LOGOUT]       = (void *(*)(CRPBaseHeader *base)) CRPLoginLogoutCast,

        [CRP_PACKET_MESSAGE__START]     = (void *(*)(CRPBaseHeader *base)) NULL,
        [CRP_PACKET_MESSAGE_TEXT]       = (void *(*)(CRPBaseHeader *base)) CRPTextMessageCast,
};

ssize_t CRPSend(uint16_t packetID, void *data, size_t length, int fd)
{
    CRPBaseHeader *header = (CRPBaseHeader *) malloc(sizeof(CRPBaseHeader) + length);
    header->magicCode = 0x464F5573;
    header->totalLength = (CRP_LENGTH_TYPE) (sizeof(CRPBaseHeader) + length);
    header->dataLength = (CRP_LENGTH_TYPE) length;
    header->packetID = packetID;
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
        perror("recv");
        return NULL;
    }
    CRPBaseHeader *packet = (CRPBaseHeader *) malloc(h.totalLength);
    ret = recv(fd, packet, h.totalLength, MSG_WAITALL);
    if (ret != h.totalLength)
    {
        perror("recv");
        free(packet);
        return NULL;
    }
    return packet;
}