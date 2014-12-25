#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <protocol/login/Login.h>
#include <string.h>
#include <protocol/login/LoginFailure.h>
#include "test.h"

int TestPacketLogin()
{
//login test
    if (!CRPLoginLoginSend(sendfd, 3, "xia", "1234561234567890"))
    {
        log_error("Login", "login返回失败\n");
        perror("");
        return 0;
    }


    CRPBaseHeader *packet;
    if (-1 == CRPRecv(&packet, recvfd))
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
    if (msgHello->username_len != 3 ||
            memcmp(msgHello->username, "xia", 3)||
            memcmp(msgHello->password, "1234561234567890", 16))
    {

        log_error("Login", "包数据错误\n");
        return 0;
    }
    log_info("Login", "Login通过\n");



}

//loginfailue test
int Testloginfail()
{

    if (!CRPLoginLoginFailureSend(sendfd, "dada"))
    {
        log_error("Loginfailue", "loginfailu返回失败\n");
        return 0;
    }



    CRPBaseHeader *packet;
    if (-1 == CRPRecv(&packet, recvfd))
    {
        log_error("Login", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_LOGIN_FAILURE)
    {
        log_error("Login", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }

    CRPPacketLoginFailure *msgHello = CRPLoginLoginFailureCast(packet);
    if ( memcmp(msgHello->reason, "dada", 4))
    {

        log_error("Loginout", "包数据错误\n");
        return 0;
    }
    log_info("Loginout", "Login通过\n");


}

//logout test
int Logout()
{

    if(!CRPLoginLogoutSend(sendfd))
    {

        log_error("Logout", "loginout错误\n");
        return 1;
    }
    log_info("Logout", "Logout通过\n");




    CRPBaseHeader *packet;
    if (-1 == CRPRecv(&packet, recvfd))
    {
        log_error("Login", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_LOGIN_LOGOUT)
    {
        log_error("Login", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_LOGIN_LOGOUT, packet->packetID);
        return 0;
    }


    log_info("Loginout", "Login通过\n");
}


int login_test()
{
    TestPacketLogin();
    Testloginfail();
    Logout();
}