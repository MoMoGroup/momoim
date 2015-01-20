#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include<stdlib.h>
#include <logger.h>
#include <protocol/CRPPackets.h>
#include<openssl/md5.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <arpa/inet.h>
#include "MainInterface.h"
#include "common.h"
#include "UpdataFriendList.h"
#include "manage_friend/friend.h"
#include "media.h"
#include "../media/audio.h"
#include "chart.h"

pthread_t ThreadKeepAlive;

CRPContext sockfd;
UserFriends *friends;
UserGroup *group;
UserInfo *CurrentUserInfo;
//gchar *uidname;
FILE *fp;
int AddFriendflag = 1;//只打开一个添加好友窗口

FriendInfo *FriendInfoHead;


void add_node(FriendInfo *node)
{
    FriendInfo *p;
    p = FriendInfoHead;

    while (p->next)
    {
        p = p->next;
    }
    p->next = node;
    node->next = NULL;
}

FriendInfo *FineNode(uint32_t uid)
{
    FriendInfo *p;
    p = FriendInfoHead;
    while (p->next)
    {
        p = p->next;

        if (p->uid == uid)
        {
            return p;
        }
    }
    log_info("NO found", "node\n");
    return 0;

}

void *keepalive(void *dada)
{
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

    UserGroup *blackgroup = UserFriendsGroupGet(friends, UGI_BLACKLIST);
    int i = 0;
    while (i < blackgroup->friendCount)
    {
        if (blackgroup->friends[i] == packet->uid)
        {
            return 0;
        }
        i++;
    }

    switch (packet->messageType)
    {

        case UMT_FILE_OFFLINE:
        {
            //好友消息声音
            PlayMusic("好友消息.wav");
            char *message = (char *) malloc(packet->messageLen);
            memcpy(message, packet->message, packet->messageLen);
            //fun();
            RecdServerFileMsg(message, packet->messageLen, packet->uid);
            free(message);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        }
        case UMT_TEXT:
        { //好友消息声音
            PlayMusic("好友消息.wav");
            char *message = (char *) malloc(packet->messageLen);
            memcpy(message, packet->message, packet->messageLen);
            //fun();
            RecdServerMsg(message, packet->messageLen, packet->uid);
            free(message);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        }
        case UMT_NEW_FRIEND:
        {
            char *mes = malloc(packet->messageLen + 1);
            mes[packet->messageLen] = 0;
            log_info("验证消息", "%s", mes);
            memcpy(mes, packet->message, packet->messageLen);
            Friend_Request_Popup(packet->uid, mes);

            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        }
    }

    free(user_data);
    return 0;
}


int friend_group_move(CRPBaseHeader *header, void *data)
{
    return 0;
}

//接收新添加好友资料的，
int new_friend_info(CRPBaseHeader *header, void *data)
{
    log_info("用户资料回复开始", "\n");
    switch (header->packetID)
    {

        case CRP_PACKET_INFO_DATA: //用户资料回复
        {
            CRPPacketInfoData *infodata = CRPInfoDataCast(header);

            char *mem = malloc(sizeof(CRPPacketInfoData));
            memcpy(mem, infodata, sizeof(CRPPacketInfoData));

            FriendInfo *node;
            node = (FriendInfo *) calloc(1, sizeof(FriendInfo));
            //node= (struct FriendInfo *)malloc(sizeof(struct FriendInfo));
            node->uid = infodata->info.uid;//添加id到结构提
            node->user = infodata->info;
            node->isonline = 0;//是否在线
            memcpy(node->user.nickName, infodata->info.nickName, sizeof(infodata->info.nickName));//添加昵称
            add_node(node);             //添加新节点


            //infodata->info;//昵称，性别等用户资料
            log_info("用户资料回复，昵称", "%s\n", infodata->info.nickName);
            FindImage((const char *) infodata->info.icon, mem, FriendListInsertEntry);//判断是否有头像
            if ((void *) infodata != header->data)
            {
                free(infodata);
            }

        }
        default:
        {
            break;
        }
    }

    return 0;//0删除
}


int servemessage(CRPBaseHeader *header, void *data)//统一处理服务器发来的消息
{
    switch (header->packetID)
    {

        //服务器通知用户下线
        case CRP_PACKET_KICK:
        {
            pthread_cancel(ThreadKeepAlive);
            pthread_join(ThreadKeepAlive, NULL);
            g_idle_add(destoryall, NULL);
            CRPClose(sockfd);

            pthread_detach(pthread_self());//安全退出当前线程
            pthread_exit(NULL);
            return 0;
        };
            //消息
        case CRP_PACKET_MESSAGE_NORMAL://不会丢的
        {
//            //好友消息声音
//            PlayMusic("好友消息.wav");
            CRPBaseHeader *dup = (CRPBaseHeader *) malloc(header->totalLength);
            memcpy(dup, header, header->totalLength);
            g_idle_add(postMessage, dup);
            return 1;
        }
            //用来分离收到语言视频在线文件请求的包
        case CRP_PACKET_NET_FRIEND_DISCOVER:
        {
            CRPPacketNETFriendDiscover *media_data = CRPNETFriendDiscoverCast(header);
            switch (media_data->reason)
            {
                //视频请求
                case CRPFDR_VEDIO:
                {
                    log_info("Serve Message", "视频请求\n");
                    CRPPacketNETFriendDiscover *video_data_copy = (CRPPacketNETFriendDiscover *) malloc(sizeof(CRPPacketNETFriendDiscover));
                    memcpy(video_data_copy, media_data, sizeof(CRPPacketNETFriendDiscover));
                    g_idle_add(treatment_request_video_discover, video_data_copy);
                    break;
                };
                    //在线文件的包
                case CRPFDR_ONLINE_FILE:
                {
                    //log_info("Serve Message", "文件请求\n");
                    //CRPNETDiscoverAcceptSend(sockfd, CountSessionId(), media_data->uid, media_data->session);
                    break;
                };
                default:
                {
                    break;
                }
            }
            if ((void *) media_data != header->data)
            {
                free(media_data);
            }
            //memcpy(audio_data,header,header->totalLength);

            //g_idle_add(Pretreatment_request_audio_net_discover,audio_data);
            break;
        };
        case CRP_PACKET_FRIEND_NOTIFY://好友列表有更新
        {
            CRPPacketFriendNotify *infodata = CRPFriendNotifyCast(header);

            switch (infodata->type)
            {
                case FNT_FRIEND_ONLINE://好友上线
                {
                    //好友上线声音
                    PlayMusic("好友上线.wav");

                    log_info("Serve Message", "好友上线\n");
                    char *mem = malloc(sizeof(CRPPacketFriendNotify));
                    memcpy(mem, infodata, sizeof(CRPPacketFriendNotify));
                    g_idle_add(OnLine, mem);
                    break;
                };

                case FNT_FRIEND_OFFLINE://好友下线
                {
                    log_info("Serve Message", "好友下线\n");
                    char *mem = malloc(sizeof(CRPPacketFriendNotify));
                    memcpy(mem, infodata, sizeof(CRPPacketFriendNotify));

                    g_idle_add(OffLine, mem);
                    //free(mem);
                    break;
                };
                case  FNT_FRIEND_INFO_CHANGED://好友资料有更新
                {
                    char *mem = malloc(sizeof(CRPPacketFriendNotify));
                    memcpy(mem, infodata, sizeof(CRPPacketFriendNotify));
                    log_info("好友资料需要更新", "UID:%u\n", infodata->uid);
                    session_id_t sessionid = CountSessionId();
                    AddMessageNode(sessionid, FriendFriendInfoChange, infodata);//注册
                    CRPInfoRequestSend(sockfd, sessionid, infodata->uid);//请求这个用户的资料
                    break;
                };
                case FNT_FRIEND_NEW://新好友
                {
                    UserGroup *grou = UserFriendsGroupGet(friends, infodata->toGid);//friends,好友分组信息
                    UserFriendsUserAdd(grou, infodata->uid);//加入这个分组

                    if (infodata->toGid != UGI_PENDING)//不是添加到pending,说明是有人加你后好友列表需要更新
                    {
                        session_id_t sessionid = CountSessionId();
                        AddMessageNode(sessionid, new_friend_info, NULL);//注册一个会话，接收新添加好友资料
                        CRPInfoRequestSend(sockfd, sessionid, infodata->uid); //请求用户资料
                    }

                    break;
                };
                case FNT_FRIEND_MOVE://说明是你添加别人后，需要移动到其它分组
                {

                    UserGroup *from_group = UserFriendsGroupGet(friends, infodata->fromGid),
                            *to_group = UserFriendsGroupGet(friends, infodata->toGid);//从哪个分组来
                    UserFriendsUserMove(from_group, to_group, infodata->uid);//加入这个分组
                    session_id_t sessionid = CountSessionId();
                    if (infodata->fromGid == UGI_PENDING)
                    {
                        AddMessageNode(sessionid, new_friend_info, NULL);//注册一个会话，接收新添加好友资料
                    }
                    else
                    {
                        AddMessageNode(sessionid, friend_group_move, NULL);
                    }
                    CRPInfoRequestSend(sockfd, sessionid, infodata->uid); //请求用户资料
                    break;
                };
                case FNT_FRIEND_DELETE:
                {
                    RemoveFriend(infodata);
                };
                default:
                {
                    break;

                }
            }
            if ((void *) infodata != header->data)
            {
                free(infodata);
            }

            break;
        };
        case CRP_PACKET_NET_NAT_REQUEST:
        {
            CRPPacketNETNATRequest *packet = CRPNETNATRequestCast(header);
            CRPPacketNETNATRequest *packetDup = (CRPPacketNETNATRequest *) malloc(sizeof(CRPPacketNETNATRequest));
            *packetDup = *packet;
            g_idle_add(ProcessAudioRequest, packetDup);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
        };
        case CRP_PACKET_OK:
        {
            log_info("服务器OK包", "%x\n", header->packetID);
            break;
        };
        default:
        {
            log_info("服务器其它消息消息", "%x\n", header->packetID);
            break;
        }
    }
    return 1;
}


int mysockfd()
{
    log_info("Test", "test\n");
//头像,好友头像
    char mulu[80] = {0};
    char mulu2[80] = {0};
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    FILE *ipfp1;
    char myip[20];
    struct in_addr inp;
    ipfp1 = fopen(checkmulu_ip, "r");
    int n = fread(myip, 1, 20, ipfp1);
    myip[n] = 0;
    log_info("IP", myip);
    inet_aton(myip, &inp);
    struct sockaddr_in server_addr = {
            .sin_family=AF_INET,
            .sin_addr.s_addr=inp.s_addr,
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
    if (header == NULL || header->packetID != CRP_PACKET_OK)
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

    if (FlagRemember == 0)
    {
        unsigned char hash[16];
        MD5((unsigned char *) pwd, strlen(pwd), hash);
        CRPLoginLoginSend(sockfd, 0, name, hash);//发送用户名密码
    }
    else
    {
        CRPLoginLoginSend(sockfd, 0, name, pwd);
    }

    header = CRPRecv(sockfd);
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        //密码错误DA
        log_info("登录失败", "登录失败\n");
        CRPPacketFailure *f = CRPFailureCast(header);
        char *mem = malloc(strlen(f->reason) + 1);
        memcpy(mem, f->reason, strlen(f->reason) - 1);
        mem[strlen(f->reason)] = 0;
        g_idle_add(DestroyLayout, mem);

        if ((void *) f != header->data)
        {
            free(f);
        }
        return 1;
    }

    if (header->packetID == CRP_PACKET_LOGIN_ACCEPT)
    {
        // log_info("登录成功", "登录成功\n");
        sleep(1);//登录动画

        //登陆成功之后开始请求资料

//        ca_gtk_play_for_widget(<#(GtkWidget*)w#>, <#(uint32_t)id, ...#>);
//        ca_gtk_proplist_set_for_event(<#(ca_proplist*)p#>, <#(GdkEvent*)e#>);

        CRPPacketLoginAccept *ac = CRPLoginAcceptCast(header);
        uint32_t uid = ac->uid;   ///拿到用户uid

        FriendInfoHead = (FriendInfo *) calloc(1, sizeof(struct FriendInfo));//创建头节点
        FriendInfoHead->flag = 1;

        free(header);
        if ((void *) ac != header->data)
        {
            free(ac);
        }


        CRPFriendRequestSend(sockfd, 1);  //请求用户好友列表

        sprintf(mulu, "%s/.momo", getpwuid(getuid())->pw_dir);
        mkdir(mulu, 0700);

        sprintf(mulu, "%s/.momo/%u", getpwuid(getuid())->pw_dir, uid);
        mkdir(mulu, 0700);
        sprintf(mulu, "%s/.momo/friend", getpwuid(getuid())->pw_dir);
        mkdir(mulu, 0700);
        sprintf(mulu, "%s/.momo/files", getpwuid(getuid())->pw_dir);
        mkdir(mulu, 0700);
        sprintf(mulu, "%s/.momo/screen", getpwuid(getuid())->pw_dir);
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
                    CRPPacketInfoData *infodata = CRPInfoDataCast(header);

                    CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//请求用户资料,通过ssionID区别

                    FriendInfo *node;
                    node = (FriendInfo *) calloc(1, sizeof(FriendInfo));
                    //node= (struct FriendInfo *)malloc(sizeof(struct FriendInfo));
                    node->uid = header->sessionID;//添加id到结构提
                    node->user = infodata->info;
                    node->isonline = infodata->isOnline;//是否在线
                    memcpy(node->user.nickName, infodata->info.nickName, sizeof(infodata->info.nickName));//添加昵称
                    add_node(node);             //添加新节点
                    if (node->uid == uid)//用户自己
                    {
                        CurrentUserInfo = &node->user;
                        node->isonline = 1;
                        log_info("user nickname:", "%s\n", infodata->info.nickName);
                    }

                    break;


                }
                case CRP_PACKET_FILE_DATA_START://服务器准备发送头像
                {
                    CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);

                    {
                        sprintf(mulu2, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, header->sessionID);

                        FriendInfo *node;
                        //node = (FriendInfo *) malloc(sizeof(FriendInfo));
                        node = FriendInfoHead;
                        while (node)
                        {
                            if (node->uid == header->sessionID)
                            {
                                char fileaname[256];
                                HexadecimalConversion(fileaname, node->user.icon);//计算一个文件名
                                if ((node->fp = fopen(fileaname, "w")) == NULL)
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


                    {
                        FriendInfo *node;
                        //node = (FriendInfo *) malloc(sizeof(FriendInfo));
                        node = FriendInfoHead;
                        while (node)
                        {

                            if (node->uid == header->sessionID)
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
                    log_info("FileDataEnd", "Session:%u\n", header->sessionID);


                    //int friendnum = 0;
                    FriendInfo *node;
                    node = FriendInfoHead;
                    while (node)
                    {
                        if (node->uid == header->sessionID)
                        {
                            fclose(node->fp);
                            node->flag = 1;//接受完毕，标志位1;
                            // friendnum++;//接受完毕的个数加1
                            break;
                        }
                        node = node->next;
                    }


                    node = FriendInfoHead;
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
                        g_idle_add(MyThread, NULL);//登陆成功调用Mythread，销毁登陆界面，加载主界面，应该在资料获取之后调用
                        loop = 0;
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
                default:
                {
                    break;
                }
            }
            free(header);


        }

        //按下记住密码的，保存用户名、密码、uid、头像路径至本地
        if ((FlagRemember == 1) && (FirstPwd == 1))
        {
            FILE *passwdfp;
            char mulu_benji[80], mulu_username[80];
            sprintf(mulu_benji, "%s/.momo", getpwuid(getuid())->pw_dir);//获取本机主目录
            mkdir(mulu_benji, 0700);
            sprintf(mulu_username, "%s/username", mulu_benji);
            passwdfp = fopen(mulu_username, "a+");
            fwrite(name, 1, 40, passwdfp);
            fwrite(pwd, 1, 16, passwdfp);
            fwrite(&uid, 1, 4, passwdfp);
            fwrite(CurrentUserInfo->icon, 1, 16, passwdfp);
            fclose(passwdfp);
        }

        AddMessageNode(0, servemessage, "");//注册服务器发来的消息

        pthread_create(&ThreadKeepAlive, NULL, keepalive, NULL);
        CRPMessageQueryOfflineSend(sockfd, CountSessionId());
        MessageLoopFunc();
    }
    log_error("DEBUG", "Unexception packet id:%hu\n", header->packetID);
    log_error("DEBUG", "ClientSockfd Done.\n");

    return 0;
}
