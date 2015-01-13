#include <protocol/info/StatusChange.h>
#include <common.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "Managegroup.h"

int changeOnlineRecv(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_OK)
    {

        g_idle_add(ShowStatus, "在线");

    }

    return 0;
}

int changeHideLineRecv(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_OK)
    {
        g_idle_add(ShowStatus, "隐身");
    }
    return 0;
}

int ChangeOnLine()
{
    session_id_t sessionid = CountSessionId();//注册会话接受服务器
    AddMessageNode(sessionid, changeOnlineRecv, NULL);
    CRPInfoStatusChangeSend(sockfd, sessionid, 0);
    return 0;
}

int ChangeHideLine()
{

    session_id_t sessionid = CountSessionId();//注册会话接受服务器
    AddMessageNode(sessionid, changeHideLineRecv, NULL);
    CRPInfoStatusChangeSend(sockfd, sessionid, 1);
    return 0;
}