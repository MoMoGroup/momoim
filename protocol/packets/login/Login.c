#include <protocol/base.h>
#include <protocol/login/Login.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base)
{
    sleep(2);
    CRPPacketLogin *packet = malloc(base->dataLength + 1);
    memcpy(packet, base->data, base->dataLength);
    ((char *) packet)[base->dataLength] = 0;
    return packet;
}

int CRPLoginLoginSend(int sockfd, const char *username, const unsigned char *password)
{
    uint8_t n = (uint8_t) strlen(username);
    CRPPacketLogin *packet = (CRPPacketLogin *) malloc(sizeof(CRPPacketLogin) + n);
    memcpy(packet->password, password, 16);
    memcpy(packet->username, username, n);
    ssize_t ret = CRPSend(CRP_PACKET_LOGIN_LOGIN, packet, sizeof(CRPPacketLogin) + n, sockfd);
    free(packet);
    return ret != -1;
}