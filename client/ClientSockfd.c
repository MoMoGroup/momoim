#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdlib.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <protocol/status/Failure.h>
#include "MainInterface.h"
#include "client.h"

pthread_t ThreadKeepAlive;

int sockfd;
UserFriends *friends;
UserGroup *group;
UserInfo userdata;
gchar *uidname;
FILE *fp;
//
//typedef struct friendinfo {
//    uint32_t sessionid;
//    int flag;
//    //用来判断是否接受成功
//    UserInfo user;
//    FILE *fp;
//
//    struct friendinfo *next;
//} friendinfo;


friendinfo *friendinfohead;
//
//
//friendinfo pnode;



void add_node(friendinfo *node) {
    friendinfo *p;
    p = friendinfohead;
    //=(friendinfo *)malloc(sizeof(struct friendinfo));
    //p=head;

    while (p->next) {
        p = p->next;
    }
    p->next = node;
    node->next = NULL;
}

void *keepalive(void *dada) {
    log_info("DEBUG", "KeepAlive Begin\n");
    while (1) {
        sleep(60);
        CRPKeepAliveSend(sockfd, 0);
    }
}

int printfun(CRPBaseHeader *header, void *data) {
    return 1;
}


gboolean postMessage(gpointer user_data) {
    CRPBaseHeader *header = (CRPBaseHeader *) user_data;

    CRPPacketMessageNormal *packet = CRPMessageNormalCast(header);
    char *message = (char *) malloc(packet->messageLen + 1);
    memcpy(message, packet->message, packet->messageLen);
    //packet->uid;
    message[packet->messageLen] = '\0';
    //fun();
    recd_server_msg(message, packet->uid);
    free(message);
    if ((void *) packet != header->data) {
        free(packet);
    }
    free(header);
    return 0;
}

int printfmessage(CRPBaseHeader *header, void *data) {
    CRPBaseHeader *dup = (CRPBaseHeader *) malloc(header->totalLength);
    memcpy(dup, header, header->totalLength);
    g_idle_add(postMessage, dup);
    return 1;
}

int backtologin(CRPBaseHeader *header, void *data) {
    log_info("销毁", "\n");
    g_idle_add(destoryall, NULL);
    close(sockfd);
//    log_info("加载", "\n");
//    g_idle_add(loadloginLayout, NULL);

    pthread_t pth = pthread_self();
    pthread_cancel(pth);
    return 0;
}

int mysockfd() {
//头像,好友头像
    char mulu[80] = {0};
    char mulu2[80] = {0};
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Connect");
        return 0;
    }
    log_info("Hello", "Sending Hello\n");
    CRPHelloSend(sockfd, 0, 1, 1, 1);
    CRPBaseHeader *header;
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK) {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 0;
    }
    log_info("Login", "Sending Login Request\n");
    unsigned char hash[16];
    MD5((unsigned char *) pwd, strlen(pwd), hash);
    CRPLoginLoginSend(sockfd, 0, name, hash);//发送用户名密码
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID == CRP_PACKET_FAILURE) {
        //密码错误DA
        log_info("登录失败", "登录失败\n");
        CRPPacketFailure *f = CRPFailureCast(header);
        char *mem = malloc(strlen(f->reason) + 1);
        memcpy(mem, f->reason, strlen(f->reason));
        mem[strlen(f->reason)] = 0;
        g_idle_add(destroyLayout, mem);
        return 1;
    }

    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT) {
        log_info("登录成功", "登录成功\n");
        //登陆成功之后开始请求资料

        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;   ///拿到用户uid

        friendinfohead = (friendinfo *) calloc(1, sizeof(struct friendinfo));//创建头节点
        friendinfohead->flag = 1;

        free(header);
        if ((void *) ac != header->data) {
            free(ac);
        }


        CRPInfoRequestSend(sockfd, 0, uid); //请求用户资料
        CRPFriendRequestSend(sockfd, 1);  //请求用户好友列表

        sprintf(mulu, "%s/.momo", getpwuid(getuid())->pw_dir);
        mkdir(mulu, 0700);

        sprintf(mulu, "%s/.momo/%u", getpwuid(getuid())->pw_dir, uid);
        mkdir(mulu, 0700);
        sprintf(mulu, "%s/.momo/friend", getpwuid(getuid())->pw_dir);

        mkdir(mulu, 0700);
        int loop = 1;
        while (loop) {
            header = CRPRecv(sockfd);
            switch (header->packetID) {
                case CRP_PACKET_FAILURE: {
                    CRPPacketFailure *failure = CRPFailureCast(header);
                    log_error("FAULT", failure->reason);
                    break;
                };
                case CRP_PACKET_INFO_DATA: //用户资料回复
                {
                    log_info("CRP_PACKET_INFO_DATA", "111\n");
                    if (header->sessionID < 10000)//小于10000,用户的自己的
                    {
                        CRPPacketInfoData *infodata = CRPInfoDataCast(header);
                        userdata = infodata->info;//放到结构提里，保存昵称，性别等资料
                        log_info("USERDATA", "Nick:%s\n", userdata.nickName);//用户昵称是否获取成功

                        CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//发送用户头像请求

                        if ((const char *) infodata != header->data) {
                            free(infodata);
                        }
                    }

                    else {
                        CRPPacketInfoData *infodata = CRPInfoDataCast(header);

                        CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//请求用户资料,通过ssionID区别

                        friendinfo *node;
                        node = (friendinfo *) calloc(1, sizeof(friendinfo));
                        //node= (struct friendinfo *)malloc(sizeof(struct friendinfo));
                        node->sessionid = header->sessionID;//添加id到结构提
                        node->user = infodata->info;
                        memcpy(node->user.nickName, infodata->info.nickName, sizeof(infodata->info.nickName));//添加昵称
                        add_node(node);             //添加新节点
                        //free(node);

                        // log_info("GROUPDATA", "Nick:%s\n", userdata.nickName);//用户昵称是否获取成功
                        // log_info("循环1", "循环1%s\n", mulu);
                    }
                    break;


                }
                case CRP_PACKET_FILE_DATA_START://服务器准备发送头像
                {
                    log_info("CRP_PACKET_FILE_DATA_START", "%u\n", header->sessionID);
                    CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);


                    if (header->sessionID < 10000)//用户的资料，准备工作，打开文件等
                    {
                        sprintf(mulu, "%s/.momo/%u/head.png", getpwuid(getuid())->pw_dir, uid);
                        if ((fp = fopen(mulu, "w")) == NULL) {
                            perror("openfile1\n");
                            exit(1);
                        }

                    }

                    else {
                        sprintf(mulu2, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, header->sessionID);

                        friendinfo *node;
                        //node = (friendinfo *) malloc(sizeof(friendinfo));
                        node = friendinfohead;
                        while (node) {
                            if (node->sessionid == header->sessionID) {
                                //node->flag=0;
                                if ((node->fp = fopen(mulu2, "w")) == NULL) {
                                    perror("openfile2\n");
                                    exit(1);
                                }
                                break;
                            }
                            node = node->next;
                        }

                    }
                    CRPOKSend(sockfd, header->sessionID);
                    if ((void *) packet != header->data) {
                        free(packet);
                    }
                    break;
                };

                case CRP_PACKET_FILE_DATA://接受头像
                {
                    log_info("CRP_PACKET_FILE_DATA", "%u\n", header->sessionID);

                    CRPPacketFileData *packet = CRPFileDataCast(header);
                    if (header->sessionID < 10000) {
                        fwrite(packet->data, 1, packet->length, fp);
                    }
                    else {
                        friendinfo *node;
                        //node = (friendinfo *) malloc(sizeof(friendinfo));
                        node = friendinfohead;
                        while (node) {

                            if (node->sessionid == header->sessionID) {
                                fwrite(packet->data, 1, packet->length, node->fp);
                                break;
                            }
                            node = node->next;
                        }
                        //free(node);

                    }
                    CRPOKSend(sockfd, header->sessionID);
                    if ((void *) packet != header->data) {
                        free(packet);
                    }

                    break;
                };

                case CRP_PACKET_FILE_DATA_END://头像接受完
                {
                    log_info("CRP_PACKET_FILE_DATA_END", "%u\n", header->sessionID);

                    CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);

                    if (header->sessionID < 10000) {
                        fclose(fp);
                    }
                    else {
                        int friendnum = 0;
                        friendinfo *node;
                        node = friendinfohead;
                        while (node) {
                            if (node->sessionid == header->sessionID) {
                                fclose(node->fp);
                                node->flag = 1;//接受完毕，标志位1;
                                friendnum++;//接受完毕的个数加1
                                break;
                            }
                            node = node->next;
                        }


                        node = friendinfohead;
                        while (node) {
                            if (node->flag == 0) {
                                break;//没有接收完
                            }
                            node = node->next;
                        }

                        if (node == NULL) {
                            log_info("开始加载主界面", "dadada\n");
                            g_idle_add(mythread, NULL);//登陆成功调用Mythread，销毁登陆界面，加载主界面，应该在资料获取之后调用
                            loop = 0;
                        }


                    }
                    if ((void *) packet != header->data) {
                        free(packet);
                    }
                    break;
                }
                case CRP_PACKET_FRIEND_DATA://分组
                {
                    log_info("CRP_PACKET_FRIEND_DATA", "555\n");

                    friends = UserFriendsDecode((unsigned char *) header->data);

                    for (int i = 0; i < friends->groupCount; ++i)//循环组
                    {
                        UserGroup *group = friends->groups + i;

                        for (int j = 0; j < group->friendCount; ++j)//循环好友

                        {
                            CRPInfoRequestSend(sockfd, group->friends[j], group->friends[j]); //请求用户资料,
                        }
                    }
                    break;
                }
            }
            free(header);


        }
        AddMessageNode(0, CRP_PACKET_OK, printfun, "daaaa");//添加事件
        AddMessageNode(0, CRP_PACKET_MESSAGE_NORMAL, printfmessage, "dfg");//添加事件
        AddMessageNode(0, CRP_PACKET_KICK, backtologin, "挤掉返回");//挤掉返回登陆界面
        pthread_create(&ThreadKeepAlive, NULL, keepalive, NULL);
        MessageLoopFunc();
    }
    log_error("DEBUG", "Unexception packet id:%hu\n", header->packetID);
    log_error("DEBUG", "ClientSockfd Done.\n");

    return 0;
}
