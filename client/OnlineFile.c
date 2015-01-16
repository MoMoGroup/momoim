#include <protocol/net/FriendDiscover.h>
#include <common.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
#include "ClientSockfd.h"
#include "MainInterface.h"

//发送文件的
int Bbb(void *data)
{
    uint32_t ip = (uint32_t) data;


    return 0;
}

//在线接收文件的
int rescv()
{
}

int kong()
{
    log_info("da", "da");
}



int askd()
{

}


int OnlineFileButtonEvent(uint8_t gid, uint32_t uid)
{
    //点击在线文件，向服务器发送IP请求
    uint32_t sessionid = CountSessionId();
    AddMessageNode(sessionid, kong, NULL);

    session_id_t sid = CountSessionId();
    AddMessageNode(sid, askd, NULL);//给sid注册一个函数，此sid

    CRPNETFriendDiscoverSend(sockfd, sessionid, gid, uid, CRPFDR_ONLINE_FILE, sid);

    return 0;
}

//在线文件，一个线程监听端口，
void *ListenOnLineTrans(void *data)
{
    int onlinesockfd, newsockfd;
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));//声明一个socketaddr结构体并清空

    if ((onlinesockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)//创建一个套接字
    {
        printf("creqate socket error\n");
        return 1;
    }


    int on = 1;
    if ((setsockopt(onlinesockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        printf("setcockopt resuse address\n");
        return 1;
    }

    //bind
    servaddr.sin_family = AF_INET;//初始化网络协议
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//指定本地绑定网卡
    // servaddr.sin_addr.s_addr= inet_addr("192");

    servaddr.sin_port = htons(8401);//指定绑定的监听端口

    if (bind(onlinesockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1)//bind绑定
    {
        perror("bind error\n");
        return 1;
    };


    //listen
    if (listen(onlinesockfd, 6) == -1)
    {
        perror("listen error\n");
        return 1;
    }
    if ((newsockfd = accept(onlinesockfd, (struct sockaddr *) NULL, NULL)) < 0)//调用accept()函数,等待客户端的链接
    {
        perror("accept");
        exit(1);
    }


}
