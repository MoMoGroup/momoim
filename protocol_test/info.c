#include <protocol/CRPPackets.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

int TestInfoQuery()
{
    if (!CRPInfoRequestSend(cs, 0, 0xabcdef01))
    {
        log_error("InfoQuery", "Send返回失败\n");
        perror("");
        return 0;
    }

    CRPBaseHeader *packet = CRPRecv(cr);
    if (packet == NULL)
    {
        log_error("InfoQuery", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_INFO_REQUEST)
    {
        log_error("InfoQuery", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }

    CRPPacketInfoRequest *msgHello = CRPInfoRequestCast(packet);
    if (msgHello->uid != 0xabcdef01)
    {

        log_error("InfoQuery", "包数据错误\n");
        return 0;
    }
    log_info("InfoQuery", "通过\n");
    free(packet);
    return 1;
}

void info_test()
{
    TestInfoQuery();
}