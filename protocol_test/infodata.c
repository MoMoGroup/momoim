#include <protocol/info/Data.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"

int TestPacketInfoData() {
    if (!CRPInfoDataSend(sendfd, 0, 0, 0x77, "baby", 'm'))
    {
        log_error("infodata", "Send返回失败\n");
        perror("");
        return 0;
    }
    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL) {
        log_error("infodata", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_INFO_DATA) {
        log_error("infodata", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }
    CRPPacketInfoData *datainfo = CRPInfoDataCast(packet);
    if (datainfo->uid != 0x77 || memcmp(datainfo->nickName, "baby", 4)
            || datainfo->sex != 'm'
            ) {
        log_error("infodata", "包数据错误\n");
        return 0;
    }
    log_info("infodata", "通过\n");
    free(packet);
    return 1;
}

int infodata_test() {
    TestPacketInfoData();
    return 1;
}