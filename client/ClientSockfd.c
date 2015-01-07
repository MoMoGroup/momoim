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
#include <protocol/message/Normal.h>
#include <protocol/friend/Notify.h>
#include <ftlist.h>
#include <protocol/base.h>
#include <protocol/info/Data.h>
#include <imcommon/friends.h>
#include "MainInterface.h"
#include "PopupWinds.h"
#include "common.h"

pthread_t ThreadKeepAlive;

CRPContext sockfd;
UserFriends *friends;
UserGroup *group;
UserInfo CurrentUserInfo;
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



void add_node(friendinfo *node)
{
    friendinfo *p;
    p = friendinfohead;
    //=(friendinfo *)malloc(sizeof(struct friendinfo));
    //p=head;

    while (p->next)
    {
        p = p->next;
    }
    p->next = node;
    node->next = NULL;
}

void *keepalive(void *dada)
{
    log_info("DEBUG", "KeepAlive Begin\n");
    while (1)
    {
        sleep(60);
        CRPKeepAliveSend(sockfd, 0);
    }
}


gboolean postMessage(gpointer user_data)
{
    CRPBaseHeader *header = (CRPBaseHeader *) user_data;
    CRPPacketMessageNormal *packet = CRPMessageNormalCast(header);

    if (packet->messageType == UMT_TEXT)
    {
        char *message = (char *) malloc(packet->messageLen + 1);
        memcpy(message, packet->message, packet->messageLen);
        //packet->uid;
        message[packet->messageLen] = '\0';
        //fun();
        recd_server_msg(message, packet->uid);
        free(message);
        if ((void *) packet != header->data)
        {
            free(packet);
        }
     }

    if( packet->messageType==UMT_NEW_FRIEND )
    {
        popup("添加请求","用户请求添加你为好友");
        CRPFriendAcceptSend(sockfd, 1, packet->uid);
    }
    return 0;
}
int shuaxin(void *data)
{

}

int newfriend(CRPBaseHeader *header,void *data)
{
    switch (header->packetID)
    {
        case CRP_PACKET_INFO_DATA: //用户资料回复
        {
            CRPPacketInfoData *infodata = CRPInfoDataCast(header);
            infodata->info;//放到结构体里，保存昵称，性别等资料
            FindImage(infodata->info.icon, infodata, shuaxin);
            return 0;//0删除
        }
    }
}

int servemessage(CRPBaseHeader *header, void *data)//统一处理服务器发来的消息
{
    switch (header->packetID)
    {

        //服务器通知下线
        case CRP_PACKET_KICK:
        {
            g_idle_add(destoryall, NULL);
            CRPClose(sockfd);
            pthread_t pth = pthread_self();
            pthread_cancel(pth);
            return 0;
        };
            //消息
        case CRP_PACKET_MESSAGE_NORMAL:
        {
            CRPBaseHeader *dup = (CRPBaseHeader *) malloc(header->totalLength);

            memcpy(dup, header, header->totalLength);
            g_idle_add(postMessage, dup);
            return 1;
        };
            //haoyou通知
        case CRP_PACKET_FRIEND_NOTIFY:
        {
            CRPPacketFriendNotify *data= CRPFriendNotifyCast(header);
            log_info("CRP_PACKET_FRIEND_NOTIFY", "%u\n",data->type);

            UserGroup* grou= UserFriendsGroupGet(friends, UGI_DEFAULT);//friends,好友分组信息

            UserFriendsUserAdd(grou, data->uid);

            session_id_t sessionid = CountSessionId();
            AddMessageNode(sessionid, newfriend, NULL);
            CRPInfoRequestSend(sockfd,sessionid , data->uid); //请求用户资料

        };
        default:
            log_info("服务器消息异常", "%u\n", header->packetID);
            return 0;
    }
}




int mysockfd()
{
//头像,好友头像
    char mulu[80] = {0};
    char mulu2[80] = {0};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
    {
        perror("Connect");
        return 0;
    }
    sockfd = CRPOpen(fd);
    CRPHelloSend(sockfd, 0, 1, 1, 1, 1);
    CRPBaseHeader *header;
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 0;
    }
    char sendKey[32], iv[32];
    CRPSwitchProtocolSend(sockfd, 1, sendKey, iv);
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_SWITCH_PROTOCOL)
    {
        log_error("SwitchProtocol", "Can not enable encrypt!\n", header->packetID);
    }
    else
    {
        CRPPacketSwitchProtocol *packet = CRPSwitchProtocolCast(header);
        CRPEncryptEnable(sockfd, sendKey, packet->key, packet->iv);
        if ((void *) packet != header->data)
        {
            free(packet);
        }
    }
    unsigned char hash[16];
    MD5((unsigned char *) pwd, strlen(pwd), hash);
    CRPLoginLoginSend(sockfd, 0, name, hash);//发送用户名密码
    header = CRPRecv(sockfd);
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        //密码错误DA
        log_info("登录失败", "登录失败\n");
        CRPPacketFailure *f = CRPFailureCast(header);
        char *mem = malloc(strlen(f->reason) + 1);
        memcpy(mem, f->reason, strlen(f->reason));
        mem[strlen(f->reason)] = 0;
        g_idle_add(DestroyLayout, mem);
        return 1;
    }

    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT)
    {
        log_info("登录成功", "登录成功\n");
        //登陆成功之后开始请求资料

        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;   ///拿到用户uid

        friendinfohead = (friendinfo *) calloc(1, sizeof(struct friendinfo));//创建头节点
        friendinfohead->flag = 1;

        free(header);
        if ((void *) ac != header->data)
        {
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
        while (loop)
        {
            header = CRPRecv(sockfd);
            switch (header->packetID)
            {
                case CRP_PACKET_FAILURE:
                {
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
                        CurrentUserInfo = infodata->info;//放到结构提里，保存昵称，性别等资料

                        CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//发送用户头像请求

                        if ((const char *) infodata != header->data)
                        {
                            free(infodata);
                        }
                    }

                    else
                    {
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


                    }
                    break;


                }
                case CRP_PACKET_FILE_DATA_START://服务器准备发送头像
                {
                    CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);

                    if (header->sessionID < 10000)//用户的资料，准备工作，打开文件等
                    {
                        sprintf(mulu, "%s/.momo/%u/head.png", getpwuid(getuid())->pw_dir, uid);
//                        char filaname[256];
//                        HexadecimalConversion(filaname,)
                        if ((fp = fopen(mulu, "w")) == NULL)
                        {
                            perror("openfile1\n");
                            exit(1);
                        }

                    }

                    else
                    {
                        sprintf(mulu2, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, header->sessionID);

                        friendinfo *node;
                        //node = (friendinfo *) malloc(sizeof(friendinfo));
                        node = friendinfohead;
                        while (node)
                        {
                            if (node->sessionid == header->sessionID)
                            {
                                //node->flag=0;
                                if ((node->fp = fopen(mulu2, "w")) == NULL)
                                {
                                    perror("openfile2\n");
                                    exit(1);
                                }
                                break;
                            }
                            node = node->next;
                        }

                    }
                    CRPOKSend(sockfd, header->sessionID);
                    if ((void *) packet != header->data)
                    {
                        free(packet);
                    }
                    break;
                };

                case CRP_PACKET_FILE_DATA://接受头像
                {

                    CRPPacketFileData *packet = CRPFileDataCast(header);
                    if (header->sessionID < 10000)
                    {
                        fwrite(packet->data, 1, packet->length, fp);
                    }
                    else
                    {
                        friendinfo *node;
                        //node = (friendinfo *) malloc(sizeof(friendinfo));
                        node = friendinfohead;
                        while (node)
                        {

                            if (node->sessionid == header->sessionID)
                            {
                                fwrite(packet->data, 1, packet->length, node->fp);
                                break;
                            }
                            node = node->next;
                        }
                        //free(node);

                    }
                    CRPOKSend(sockfd, header->sessionID);
                    if ((void *) packet != header->data)
                    {
                        free(packet);
                    }

                    break;
                };

                case CRP_PACKET_FILE_DATA_END://头像接受完
                {

                    CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);

                    if (header->sessionID < 10000)
                    {
                        fclose(fp);
                    }
                    else
                    {
                        int friendnum = 0;
                        friendinfo *node;
                        node = friendinfohead;
                        while (node)
                        {
                            if (node->sessionid == header->sessionID)
                            {
                                fclose(node->fp);
                                node->flag = 1;//接受完毕，标志位1;
                                friendnum++;//接受完毕的个数加1
                                break;
                            }
                            node = node->next;
                        }


                        node = friendinfohead;
                        while (node)
                        {
                            if (node->flag == 0)
                            {
                                break;//没有接收完
                            }
                            node = node->next;
                        }

                        if (node == NULL)
                        {
                            g_idle_add(mythread, NULL);//登陆成功调用Mythread，销毁登陆界面，加载主界面，应该在资料获取之后调用
                            loop = 0;
                        }


                    }
                    if ((void *) packet != header->data)
                    {
                        free(packet);
                    }
                    break;
                }
                case CRP_PACKET_FRIEND_DATA://分组
                {

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
        AddMessageNode(0, servemessage, "");//注册服务器发来的消息

        pthread_create(&ThreadKeepAlive, NULL, keepalive, NULL);
        MessageLoopFunc();
    }
    log_error("DEBUG", "Unexception packet id:%hu\n", header->packetID);
    log_error("DEBUG", "ClientSockfd Done.\n");

    return 0;
}
