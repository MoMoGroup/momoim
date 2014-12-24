#include <protocol/base.h>
#include <protocol/login/Login.h>
#include <protocol/packets.h>
#include <string.h>
#include <stdlib.h>


CRPPacketLogin *CRPLoginLoginCast(CRPBaseHeader *base)
{
    return (CRPPacketLogin *) base->data;
}

int CRPLoginLoginSend(int sockfd, uint8_t username_len, char *username, char *password)
{
    CRPPacketLogin *packet = (CRPPacketLogin *) malloc(sizeof(CRPPacketLogin) + username_len);
    packet->username_len = username_len;
    memcpy(packet->password, password, 16);
    memcpy(packet->username, username, username_len);
    ssize_t ret = CRPSend(CRP_PACKET_LOGIN_LOGIN, packet, sizeof(CRPPacketLogin) + username_len, sockfd);
    free(packet);
    return ret != -1;
}