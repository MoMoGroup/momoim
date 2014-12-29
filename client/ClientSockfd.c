#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include<stdlib.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <protocol/base.h>
#include <string.h>
#include <protocol/info/Data.h>
#include <protocol/status/Failure.h>
#include <pwd.h>
#include <sys/stat.h>
#include <imcommon/friends.h>
#include <errno.h>
#include <grp.h>
#include "MainInterface.h"


int sockfd;
UserFriends *friends;
UserGroup *group;
CRPPacketInfoData userdata, groupdata;
gchar *uidname;
FILE *fp, *fp2;
//头像,好友头像
char mulu[80] = {0};
char mulu2[80] = {0};
int touxiang = 0, liebiao = 0;
int j;
//groupflag
int fcount = 0;//好友数量



int mysockfd()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=htonl(INADDR_LOOPBACK),
            .sin_port=htons(8014)
    };
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
    {
        perror("Connect");
        return 0;
    }
    log_info("Hello", "Sending Hello\n");
    CRPHelloSend(sockfd, 0, 1, 1, 1);
    CRPBaseHeader *header;
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK)
    {
        log_error("Hello", "Recv Packet:%d\n", header->packetID);
        return 0;
    }

    log_info("Login", "Sending Login Request\n");
    const gchar *name, *pwd;
    name = gtk_entry_get_text(GTK_ENTRY(username));
    pwd = gtk_entry_get_text(GTK_ENTRY(passwd));

    unsigned char hash[16];
    MD5((unsigned char *) pwd, 1, hash);
    CRPLoginLoginSend(sockfd, 0, name, hash);//发送用户名密码


    header = CRPRecv(sockfd);
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        //密码错误
        log_info("登录失败", "登录失败\n");



        //gtk_container_add(GTK_CONTAINER (window), layout);
        g_idle_add(destroyLayout, NULL);
        return 1;
    }
    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT)
    {
        log_info("登录成功", "登录成功\n");
        //登陆成功之后开始请求资料
        sprintf(mulu, "%s/momo", getpwuid(getuid())->pw_dir);
        mkdir(mulu,0700);
        g_idle_add(mythread, NULL);//登陆成功调用Mythread，销毁登陆界面，加载主界面，应该在资料获取之后调用
        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;   ///拿到用户uid
        if (ac != header->data)
        {
            free(ac);
        }
        free(header);

        CRPInfoRequestSend(sockfd, 0, uid); //请求用户资料
        CRPFriendRequestSend(sockfd, 1);  //请求用户好友列表
        while (errno < 10000)
        {
            header = CRPRecv(sockfd);
            switch (header->packetID)
            {
                case CRP_PACKET_INFO_DATA: //用户资料回复
                {
                    if (header->sessionID < 10000)//小于10000,用户的自己的
                    {
                        CRPPacketInfoData *infodata = CRPInfoDataCast(header);
                        memcpy(&userdata, infodata, sizeof(CRPPacketInfoData));//放到结构提里，保存昵称，性别等资料

                        log_info("USERDATA", "Nick:%s\n", userdata.nickName);//用户昵称是否获取成功
                        CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->icon);//发送头像请求

                        if (infodata != header->data)
                        {
                            free(infodata);
                        }
                    }

                    else
                    {
                        CRPPacketInfoData *infodata = CRPInfoDataCast(header);
                        memcpy(&groupdata, infodata, sizeof(CRPPacketInfoData));

                        CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->icon);//请求用户头像,通过ssionID区别
                        log_info("循环1", "循环1%s\n", mulu);
                    }
                    break;


                }
                case CRP_PACKET_FILE_DATA_START://服务器准备发送头像
                {
                    if (header->sessionID < 10000)//用户的资料，准备工作，打开文件等
                    {
                        sprintf(mulu, "%s/momo/head.png", getpwuid(getuid())->pw_dir);
                        log_info("路径", "%s\n", mulu);
                        if ((fp = fopen(mulu, "w")) == NULL)
                        {
                            perror("openfile\n");
                        }
                        CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);
                        log_info("Icon", "%lu bytes will be received\n", packet->dataLength);
                        if (packet != header->data)
                        {
                            free(packet);
                        }
                    }
                    else
                    {
                        sprintf(mulu2, "%s/momo/%u", getpwuid(getuid())->pw_dir, header->sessionID);
                        log_info("路径", "%s\n", mulu);
                        if ((fp2 = fopen(mulu2, "w")) == NULL)
                        {
                            perror("openfile\n");
                        }
                        CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);
                        log_info("Icon", "%lu bytes will be received\n", packet->dataLength);
                        if (packet != header->data)
                        {
                            free(packet);
                        }
                    }


                    break;
                };
                case CRP_PACKET_FILE_DATA://接受头像
                    if (header->sessionID < 10000)
                    {
                        fwrite(header->data, 1, header->dataLength, fp);
                    }
                    else
                    {
                        fwrite(header->data, 1, header->dataLength, fp2);
                    }

                    break;

                case CRP_PACKET_FILE_DATA_END:
                {
                    CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);

                    if (header->sessionID < 10000)
                    {
                        fclose(fp);
                    }
                    else
                    {
                       fclose(fp2);
                    }
                    if (packet != header->data)
                    {
                        free(packet);
                    }
                    break;
                }
                case CRP_PACKET_FRIEND_DATA:
                {


                    UserFriends *friends = UserFriendsDecode((unsigned char *) header->data);
                    log_info("Friends", "Group Count:%d\n", friends->groupCount);
                    for (int i = 0; i < friends->groupCount; ++i)//循环组
                    {
                        UserGroup *group = friends->groups + i;
                        log_info(group->groupName, "GroupID:%d\n", group->groupId);
                        log_info(group->groupName, "FriendCount:%d\n", group->friendCount);
                        for (int j = 0; j < group->friendCount; ++j)//循环好友
                        {
                            CRPInfoRequestSend(sockfd, group->friends[j], group->friends[j]); //请求用户资料,
                            log_info(group->groupName, "Friend:%u\n", group->friends[j]);

                        }
                    }
                    UserFriendsFree(friends);
                    break;
                }
            }
            free(header);


        }
    }
    free(header);
    return 0;
}
