#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"

int TestPacketHello()
{
    if (!CRPHelloSend(sendfd, 0x78, 0x21, 0x32))
    {
        log_error("Hello", "Send返回失败\n");
        return 0;
    }

    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("Hello", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_HELLO)
    {
        log_error("Hello", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }

    CRPPacketHello *msgHello = CRPHelloCast(packet);
    if (msgHello->protocolVersion != 0x78 ||
        msgHello->clientVersion != 0x21 ||
        msgHello->sessionState != 0x32)
    {

        log_error("Hello", "包数据错误\n");
        return 0;
    }
    log_info("Hello", "通过\n");
    free(packet);
    return 1;
}

//loginfailue test
int Testfail()
{

    if (!CRPFailureSend(sendfd, "dada"))
    {
        log_error("Loginfailue", "loginfailu返回失败\n");
        return 0;
    }


    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("Login", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_FAILURE)
    {
        log_error("Login", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }

    CRPPacketFailure *msgHello = CRPFailureCast(packet);
    if (memcmp(msgHello->reason, "dada", 4))
    {

        log_error("Loginout", "包数据错误\n");
        return 0;
    }
    log_info("Loginout", "Login通过\n");
    free(packet);
    return 1;
}

int status_test()
{
    TestPacketHello();
    Testfail();
    return 1;
}