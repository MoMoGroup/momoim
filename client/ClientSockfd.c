#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <protocol/status/Hello.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <protocol/base.h>
#include <stdlib.h>
#include <string.h>
#include <protocol/info/Data.h>
#include <protocol/status/Failure.h>
#include <pwd.h>
#include <sys/stat.h>
#include <imcommon/friends.h>
#include "MainInterface.h"

extern int flag;
extern GtkWidget *loginLayout, *pendingLayout;
extern GtkWidget *username, *passwd;
extern GtkWidget *window;
extern int sockfd;
UserFriends *friends;
UserGroup *group;
CRPPacketInfoData userdata, groupdata;
gchar *uidname;
FILE *fp, **fp2;
//头像,好友头像
char mulu[80] = {0};
char mulu2[80] = {0};
int touxiang = 0, liebiao = 0;
int j;
//groupflag
int fcount = 0;//好友数量

gboolean mythread(gpointer user_data) {
    gtk_widget_destroy(window);
    maininterface();
    return 0;
}

int mysockfd() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    log_info("Hello", "Waiting OK\n");
    header = CRPRecv(sockfd);
    if (header->packetID != CRP_PACKET_OK) {
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
    if (header->packetID == CRP_PACKET_FAILURE) {
        //密码错误
        log_info("登录失败", "登录失败\n");
        flag = 1;
        gtk_widget_destroy(pendingLayout);
        //gtk_container_add(GTK_CONTAINER (window), layout);
        gtk_widget_show_all(loginLayout);
        return 1;
    }
    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT) {
        log_info("登录成功", "登录成功\n");

        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;
        if (ac != header->data) {
            free(ac);
        }
        free(header);
        CRPInfoRequestSend(sockfd, 0, uid); //请求用户资料
        CRPFriendRequestSend(sockfd, 1);  //请求用户好友列表
        while (1) {
            log_info("循环中", "循环\n");
            header = CRPRecv(sockfd);
            switch (header->packetID) {
                case CRP_PACKET_INFO_DATA: {
                    if (header->sessionID < 10000) {
                        CRPPacketInfoData *ac = CRPInfoDataCast(header);
                        memcpy(&userdata, ac, sizeof(CRPPacketInfoData));
                        CRPFileRequestSend(sockfd, 2, 0, ac->icon);
                        if (ac != header->data)
                            free(ac);
                    }
                    if (header->sessionID >= 10000) {
                        CRPPacketInfoData *ac = CRPInfoDataCast(header);
                        memcpy(&groupdata, ac, sizeof(CRPPacketInfoData));
                        CRPFileRequestSend(sockfd, group->friends[j], 0, ac->icon);
                        log_info("循环1", "1\n");
                    }
                    free(header);
                    break;
                }
                case CRP_PACKET_FILE_DATA_START: {

                    //log_info("登录1", getpwuid(getuid())->pw_dir);

                    if (header->sessionID < 10000) {
//                        memcpy(mulu, getpwuid(getuid())->pw_dir, 80);
//                        strcat(mulu, "/.momo");
//                       mkdir(mulu, 0700);
//                        log_info("登录1", mulu);
//                        strcat(mulu, "mm");
                        sprintf(mulu, "%s%s%s", getpwuid(getuid())->pw_dir, "/.momo/", "mm");
                        if ((fp = fopen(mulu, "w")) == NULL) {
                            perror("\nopenfileerror");
                            exit(1);
                        }
                    }
                    if (header->sessionID >= 10000) {
                        //memcpy(mulu2, getpwuid(getuid())->pw_dir, 80);
                        //strcat(mulu2, "/.momo/");///home/lh/.momo/7482748
                        //mkdir(mulu2, 0700);
                        sprintf(mulu2, "%s%s%u", getpwuid(getuid())->pw_dir, "/.momo/", header->sessionID);
                        //strcat(mulu2, header->sessionID);
                        for (int num = 0; num < fcount; num++) {
                            if ((fp2[num] = fopen(mulu2, "w")) == NULL) {
                                perror("\nopenfileerror");
                                exit(1);
                            }
                            else {
                                memset(mulu2, '\0', 80);
                            }
                        }
                    }

                    break;
                }
                case CRP_PACKET_FILE_DATA: {
                    log_info("登录2", "登录2\n");
                    if (header->sessionID < 10000) {
                        fwrite(header->data, 1, header->dataLength, fp);
                        log_info("Icon", "Recv data %lu bytes.\n", header->dataLength);
                    }
                    if (header->sessionID >= 10000) {
                        log_info("循环3", "3\n");
                        fwrite(header->data, 1, header->dataLength, fp2);
                        log_info("Icon", "Recv data %lu bytes.\n", header->dataLength);
                    }
                    break;

                }
                case CRP_PACKET_FILE_DATA_END: {
                    log_info("登录3", "登录3\n");
                    CRPPacketFileDataEnd *packet = CRPFileDataEndCast(header);
                    if (header->sessionID < 10000) {
                        if (packet->code == 0) {
                            log_info("Icon", "Recv Successful\n");
                            touxiang = 1;
                            fclose(fp);
                        }
                        else {
                            log_info("Icon", "Recv Fail with code %d", (int) packet->code);
                        }
                    }
                    if (header->sessionID >= 10000) {
                        log_info("循环4", "4\n");
                    }
                    if (packet != header->data)
                        free(packet);
                    break;
                }
                case CRP_PACKET_FRIEND_DATA: {
                    log_info("登录4", "登录4\n");
                    if (header->sessionID < 10000) {
                        liebiao = 1;
                    }
                    friends = UserFriendsDecode((unsigned char *) header->data);
                    for (int i = 0; i < friends->groupCount; ++i) {
                        group = friends->groups + i;
                        log_info(group->groupName, "GroupID:%d\n", group->groupId);
                        log_info(group->groupName, "FriendCount:%d\n", group->friendCount);
                        for (j = 0; j < group->friendCount; ++j) {
                            fcount++;
                            log_info(group->groupName, "Friend:%u\n", group->friends[j]);
                            CRPInfoRequestSend(sockfd, group->friends[j], group->friends[j]);
                        }
                    }
                    UserFriendsFree(friends);
                    //uint32_t *friendsid = friends->groups->friends;
                    //CRPInfoRequestSend(sockfd, 2, friendsid[1]);
                    break;
                };

                case CRP_PACKET_FAILURE: {
                    CRPPacketFailure *failure = CRPFailureCast(header);
                    log_info("DEBUGFailure", "%s\n", failure->reason);
                    if (failure != header->data)
                        free(failure);
                    break;
                }
                    break;

            };
            if (touxiang == 1 && liebiao == 1) {
                gdk_threads_add_idle(mythread, NULL);
            }
        }
    }
    free(header);
    return 0;
}
/*                log_info("Friends", "Group Count:%d\n", friends->groupCount);
                for (int i = 0; i < friends->groupCount; ++i)
                {
                    UserGroup *group = friends->groups + i;
                    log_info(group->groupName, "GroupID:%d\n", group->groupId);
                    log_info(group->groupName, "FriendCount:%d\n", group->friendCount);
                    for (int j = 0; j < group->friendCount; ++j)
                    {
                        log_info(group->groupName, "Friend:%u\n", group->friends[j]);
                    }
                }
                UserFriendsFree(friends);*/

/*uint32_t uid = ac->uid;
if (ac != header->data) {
    free(ac);
}

CRPInfoRequestSend(sockfd, 0, uid);
free(header);
header = CRPRecv(sockfd);
if (header->packetID == CRP_PACKET_INFO_DATA) {
    //log_info("User", "Nick%s\n", (CRPInfoDataCast(header)->nickName));
    CRPPacketInfoData *ac = CRPInfoDataCast(header);
    memcpy(&userdata, ac, sizeof(CRPPacketInfoData));
    if (ac != header->data)
        free(ac);
    //uidname=CRPInfoDataCast(header)->nickName;
    //g_print(uidname);
    free(header);
    gdk_threads_add_idle(mythread, NULL);
}
else {
    log_info("User", "Info Failure\n");
}
return 0;

销毁loginlayout对话框
gtk_container_add(GTK_CONTAINER(window), pendingLayout);

gtk_widget_show_all(pendingLayout);*/
