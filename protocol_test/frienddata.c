#include <protocol/CRPPackets.h>
#include <protocol/friend/Data.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

/*int TestPacketFriendData() {

    UserGroup mygroup={0x88,"child",4,0x22};
    UserFriends myfriend={2,&mygroup};
    if (!CRPFriendDataSend(sendfd,&myfriend)) {
        log_error("fridata", "Send返回失败\n");
        perror("");
        return 0;
    }

    CRPBaseHeader *packet = CRPRecv(recvfd);
    if (packet == NULL) {
        log_error("fridata", "Recv返回失败\n");
        return 0;
    }
    if (packet->packetID != CRP_PACKET_FRIEND_DATA) {
        log_error("fridata", "packetID错误。(预期的ID:%d，收到的ID:%d)\n", CRP_PACKET_HELLO, packet->packetID);
        return 0;
    }
    CRPPacketFriendData *friendinfo=CRPFriendDataCast(packet);
    if (friendinfo->data!=)
    {

        log_error("message", "包数据错误\n");
        return 0;
    }
    log_info("message", "通过\n");
    free(packet);
    return 1;
}*/
int fridata_test() {
    //TestPacketFriendData(); //调用检测函数
    return 1;
}