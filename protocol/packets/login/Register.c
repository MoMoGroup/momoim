#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <string.h>
#include <stdlib.h>
#include <protocol/login/Register.h>


CRPPacketLoginRegister *CRPLoginRegisterCast(CRPBaseHeader *base)
{
    CRPPacketLoginRegister *packet = (CRPPacketLoginRegister *) base->data;
    packet = (CRPPacketLoginRegister *) malloc(
            sizeof(CRPPacketLoginRegister)
                    + packet->usernameLength + 1
                    + packet->nicknameLength + 1
    );
    char *p = (char *) packet;
    memcpy(p, base->data, sizeof(CRPPacketLoginRegister) - sizeof(packet->username) - sizeof(packet->nickname));
    p += sizeof(CRPPacketLoginRegister) - sizeof(packet->username) - sizeof(packet->nickname);

    packet->username = sizeof(CRPPacketLoginRegister) + (char *) packet;
    packet->username[packet->usernameLength] = 0;
    packet->nickname = packet->username + packet->usernameLength + 1;
    packet->nickname[packet->nicknameLength] = 0;

    memcpy(packet->username, base->data + sizeof(CRPPacketLoginRegister) - sizeof(packet->username) - sizeof(packet->nickname), packet->usernameLength);
    memcpy(packet->nickname,
            base->data +
                    sizeof(CRPPacketLoginRegister) - sizeof(packet->username) - sizeof(packet->nickname) +
                    packet->usernameLength,

            packet->nicknameLength);
    return packet;
}

/**
* 发送登陆包
*/
int CRPLoginRegisterSend(
        int sockfd,
        uint32_t sessionID,
        const char *username,
        const unsigned char *password,
        const char *nickname)
{
    uint8_t userLen = (uint8_t) strlen(username),
            nickLen = (uint8_t) strlen(nickname);
    char *mem = (char *) malloc(
            sizeof(CRPPacketLoginRegister)
                    - sizeof(NILOBJ(CRPPacketLoginRegister)->username)
                    - sizeof(NILOBJ(CRPPacketLoginRegister)->nickname)
                    + userLen
                    + nickLen);
    char *p = mem;
    memcpy(p, &userLen, sizeof(userLen));
    p += sizeof(userLen);
    memcpy(p, &nickLen, sizeof(nickLen));
    p += sizeof(nickLen);
    memcpy(p, password, sizeof(NILOBJ(CRPPacketLoginRegister)->password));
    p += sizeof(NILOBJ(CRPPacketLoginRegister)->password);
    memcpy(p, username, userLen);
    p += userLen;
    memcpy(p, nickname, nickLen);
    p += nickLen;
    ssize_t ret = CRPSend(CRP_PACKET_LOGIN_REGISTER, sessionID, mem, p - mem, sockfd);
    free(mem);
    return ret != -1;
}