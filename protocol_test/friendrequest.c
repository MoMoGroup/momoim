#include <protocol/friend/Request.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

int TestPacketFriRequest()
{
    if (!CRPFriendRequestSend(sendfd, 0))
    {
        log_error("frirequest", "Send返回失败\n");
        perror("");
        return 0;
    }
    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL)
    {
        log_error("frirequest", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_FRIEND_REQUEST)
    {
        log_error("frirequest", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }
    log_info("frirequest", "通过\n");
    free(packet);
    return 1;
}

int frirequest_test()
{
    TestPacketFriRequest();
    return 1;
}