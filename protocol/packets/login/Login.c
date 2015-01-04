#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base)
{
    CRPPacketLogin *packet = malloc(base->totalLength - sizeof(CRPBaseHeader) + 1);
    memcpy(packet, base->data, base->totalLength - sizeof(CRPBaseHeader));
    ((char *) packet)[base->totalLength - sizeof(CRPBaseHeader)] = 0;
    return packet;
}

int CRPLoginLoginSend(int sockfd, uint32_t sessionID, const char *username, const unsigned char *password)
{
    uint8_t n = (uint8_t) strlen(username);
    CRPPacketLogin *packet = (CRPPacketLogin *) malloc(sizeof(CRPPacketLogin) + n);
    memcpy(packet->password, password, 16);
    memcpy(packet->username, username, n);
    ssize_t ret = CRPSend(CRP_PACKET_LOGIN_LOGIN, sessionID, packet, sizeof(CRPPacketLogin) + n, sockfd);
    free(packet);
    return ret != -1;
}