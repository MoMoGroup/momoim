#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"

int TestPacketMessage()
{
    if (!CRPMessageNormalSend(cs, 0, UMT_TEXT, 0x21, 8, "JDKAJDKA")) {
        log_error("message", "Send返回失败\n");
        perror("");
        return 0;
    }

    CRPBaseHeader *packet = CRPRecv(cr);
    if (packet == NULL) {
        log_error("message", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_MESSAGE_NORMAL) {
        log_error("message", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }
//包数据检测正确与否
    CRPPacketMessageNormal *msgHello = CRPMessageNormalCast(packet);
    if (msgHello->uid != 0x21 ||
            msgHello->messageType != UMT_TEXT ||
            msgHello->messageLen != 8 ||
            memcmp(msgHello->message, "JDKAJDKA", 8)
            ) {

        log_error("message", "包数据错误\n");
        return 0;
    }
    log_info("message", "通过\n");
    free(packet);
    return 1;
}

int message_test()
{
    TestPacketMessage(); //调用检测函数
    return 1;
}