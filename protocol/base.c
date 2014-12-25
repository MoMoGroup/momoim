#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>

#include <protocol/base.h>

ssize_t CRPSend(int packetID, void *data, size_t length, int fd)
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

ssize_t CRPRecv(CRPBaseHeader **header, int fd)
{
    CRPBaseHeader h;
    ssize_t ret;

    ret = recv(fd, &h, sizeof(CRPBaseHeader), MSG_WAITALL | MSG_PEEK);
    if (ret != sizeof(CRPBaseHeader) || h.magicCode != 0x464F5573)
    {
        perror("recv");
        return -1;
    }
    CRPBaseHeader *packet = (CRPBaseHeader *) malloc(h.totalLength);
    ret = recv(fd, packet, h.totalLength, MSG_WAITALL);
    if (ret != h.totalLength)
    {
        perror("recv");
        free(packet);
        return -1;
    }

    *header = packet;
    return ret;
}