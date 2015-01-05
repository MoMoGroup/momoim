#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"

int TestPacketLogin()
{
//login test
    if (!CRPLoginLoginSend(sendfd, 0, "xia", (const unsigned char *) "1234561234567890"))
    {
        log_error("Login", "login返回失败\n");
        perror("");
        return 0;
    }


    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("Login", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_LOGIN_LOGIN)
    {
        log_error("Login", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_LOGIN_LOGIN, packet->packetID);
        return 0;
    }

    CRPPacketLogin *msgHello = CRPLoginLoginCast(packet);
    if (memcmp(msgHello->username, "xia", 3) ||
            memcmp(msgHello->password, "1234561234567890", 16))
    {

        log_error("Login", "包数据错误\n");
        return 0;
    }
    log_info("Login", "Login通过\n");
    free(packet);
    return 0;
}

int LogAccept()
{

    if (!CRPLoginAcceptSend(sendfd, 0, 0x12345678))
    {
        log_error("LoginAccept", "login返回失败\n");
        return 0;
    }


    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("LoginAccept", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_LOGIN_ACCEPT)
    {
        log_error("LoginAccept", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_LOGIN_ACCEPT, packet->packetID);
        return 0;
    }

    CRPPacketLoginAccept *msgHello = CRPLoginAcceptCast(packet);
    if (msgHello->uid != 0x12345678)
    {

        log_error("LoginAccept", "包数据错误\n");
        return 0;
    }
    log_info("LoginAccept", "LoginAccept通过\n");
    free(packet);
    return 0;
}

//logout test
int Logout()
{

    if (!CRPLoginLogoutSend(sendfd, 0))
    {

        log_error("Logout", "loginout错误\n");
        return 1;
    }
    log_info("Logout", "Logout通过\n");


    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("Login", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_LOGIN_LOGOUT)
    {
        log_error("Login", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_LOGIN_LOGOUT, packet->packetID);
        return 0;
    }

    free(packet);
    log_info("Loginout", "Login通过\n");
    return 0;
}


int login_test()
{
    TestPacketLogin();
    LogAccept();
    Logout();
    return 0;
}