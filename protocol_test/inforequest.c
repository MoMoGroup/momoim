#include <protocol/info/Request.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

int TestPacketRequest() {

    if (!CRPInfoRequestSend(sendfd, 0, 0x88)) {
        log_error("inforequest", "Send返回失败\n");
        perror("");
        return 0;

    }
    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL) {
        log_error("inforequest", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_INFO_REQUEST) {
        log_error("inforequest", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }
    CRPPacketInfoRequest *inforequest = CRPInfoRequestCast(packet);
    if (inforequest->uid != 0x88) {
        log_error("inforequest", "包数据错误\n");
        return 0;
    }
    log_info("inforequest", "通过\n");
    free(packet);
    return 1;
}

int inforequest_test() {
    TestPacketRequest();
    return 1;
}