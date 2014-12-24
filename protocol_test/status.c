#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/packets.h>
#include <stdio.h>
#include "test.h"

int TestPacketHello()
{
    if (!CRPHelloSend(sendfd, 0x78, 0x21, 0x32))
    {
        log_error("Hello", "Send返回失败\n");
        perror("");
        return 0;
    }

    CRPBaseHeader *packet;
    if (-1 == CRPRecv(&packet, recvfd))
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
}

int status_test()
{
    TestPacketHello();
}