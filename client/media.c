#include <protocol.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <logger.h>
#include <common.h>
#include <arpa/inet.h>
#include "PopupWinds.h"
#include "media.h"
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "../media/video.h"
#include "../media/audio.h"

//这个结构体用来保存请求音视频的记录
struct log_request_friend_discover the_log_request_friend_discover;

//服务器送达数据包错误时弹窗
static int popup_audio(gpointer p)
{
    popup("错误", "无法建立连接");
    return 0;
}

//对方拒绝请求时的弹窗
static int popup_audio_request_refuse(gpointer p)
{
    popup("消息", "对方已拒绝您的音频请求");
    return 0;
}

//对方同意请求时的弹窗
static int popup_audio_request_accept(gpointer p)
{
    popup("消息", "对方已接受了您的音频请求");
    return 0;
}

//对方拒绝请求时的弹窗
static int popup_video_request_refuse(gpointer p)
{
    popup("消息", "对方已拒绝您的视频请求");
    return 0;
}

//对方同意请求时的弹窗
static int popup_video_request_accept(gpointer p)
{
    popup("消息", "对方已接受了您的视频请求");
    return 0;
}
////提示弹窗
//int popup_request_num_limit(gpointer p){
//    popup("消息","同一时间只能对一个好友发起请求哦");
//    return 0;
//}

//处理服务器发送net_friend_discover这个包的反馈函数
//貌似音视频都可以用这个函数啊
int deal_video_dicover_server_feedback(CRPBaseHeader *header, u_int32_t uid)
{
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        g_idle_add(popup_audio, NULL);
        //the_log_request_friend_discover.uid=-1;
        //the_log_request_friend_discover.requset_reason=-1;
    }
    return 0;
}

//主动发起方,等待接受方建立连接
void *AudioWaitConnection(struct AudioDiscoverProcessEntry *entry)
{
    pthread_detach(pthread_self());
    int sockSender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int isServerDetected = 0, isRecvPipeOK = 0;
    struct sockaddr_in serverNatService;
    socklen_t serverAddrLen = sizeof(serverNatService);
    getpeername(sockfd->fd, (struct sockaddr *) &serverNatService, &serverAddrLen);
    serverNatService.sin_port = htons(8015);
    int tryTimes = 0;
    while (!entry->peerReady || !isRecvPipeOK)
    {
        log_info("RecvPipe", "%d\n", isRecvPipeOK);
        if (!isServerDetected)
        {
            sendto(sockSender, entry->peerKey, 32, 0, (struct sockaddr *) &serverNatService, serverAddrLen);
        }
        sendto(sockSender, entry->peerKey, 32, 0, (struct sockaddr *) &entry->addr, sizeof(entry->addr));
        log_info("SendKey", "To %s:%hu\n", inet_ntoa(entry->addr.sin_addr), ntohs(entry->addr.sin_port));
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockSender, &set);
        struct timeval timeout = {
                .tv_sec=1
        };
        int n;
        n = select(sockSender + 1, &set, NULL, NULL, &timeout);
        if (n > 0)
        {
            char buffer[32];
            struct sockaddr_in opaddr;
            socklen_t opAddrLen;
            n = (int) recvfrom(sockSender, buffer, 32, 0, (struct sockaddr *) &opaddr, &opAddrLen);
            if (n == 32)
            {
                if (memcmp(buffer, entry->peerKey, 32) == 0//与本地key相等是服务器返回数据
                    || memcmp(buffer, (uint8_t[32]) {0}, 32) == 0)
                {
                    log_info("Discover", "Server Found\n");
                    isServerDetected = 1;
                }
                else if (memcmp(buffer, entry->key, 32) == 0)
                {
                    log_info("Discover", "Peer Found\n");
                    isRecvPipeOK = 1;
                    memcpy(&entry->addr, &opaddr, opAddrLen);
                    CRPNETNATReadySend(sockfd, entry->localSession, entry->peerUid, entry->peerSession);
                }
                else
                {
                    char hexKey[65] = {0};
                    for (int i = 0; i < 32; ++i)
                    {
                        sprintf(hexKey + i * 2, "%02x", (int) buffer[i]);
                    }
                    log_info("Key", "WrongKey%s\n", hexKey);
                }

            }
            else
            {
                perror("recv");
            }
        }
        else
        {
            if (++tryTimes >= 10)//10次重试未成功
            {
                free(entry);
                return 0;
            }
        }
    }
    StartAudioChat_Send(sockSender, &entry->addr);
    free(entry);
}

//主动发起音频方处理消息句柄
int processNatDiscoveredOnAudio(CRPBaseHeader *header, void *data)
{
    struct AudioDiscoverProcessEntry *entry = (struct AudioDiscoverProcessEntry *) data;
    switch (header->packetID)
    {
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *packet = CRPFailureCast(header);
            log_error("NatDiscover", "error%s\n", packet->reason);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            pthread_cancel(entry->workerThread);
            free(entry);
            return 0;
        };
        case CRP_PACKET_NET_NAT_READY:
        {
            entry->peerReady = 1;
            return 0;
        };
        case CRP_PACKET_NET_NAT_ACCEPT:
        {
            CRPPacketNETNATAccept *packet = CRPNETNATAcceptCast(header);
            memcpy(entry->peerKey, packet->key, 32);
            entry->peerKeySet = 1;
            entry->peerSession = packet->session;
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            if (entry->addr.sin_port)
            {
                pthread_create(&entry->workerThread, NULL, (void *(*)(void *)) AudioWaitConnection, entry);
            }
            break;
        };
        case CRP_PACKET_NET_NAT_DETECTED:
        {
            CRPPacketNATDetected *packet = CRPNATDetectedCast(header);
            entry->addr.sin_family = AF_INET;
            entry->addr.sin_addr.s_addr = packet->ipv4;
            entry->addr.sin_port = packet->port;

            if ((void *) packet != header->data)
            {
                free(packet);
            }
            if (entry->peerKeySet)
            {
                pthread_create(&entry->workerThread, NULL, (void *(*)(void *)) AudioWaitConnection, entry);
            }
            break;
        };
        case CRP_PACKET_OK:
        {
            if (!entry->messageSent)
            {
                char hexKey[65] = {0};
                for (int i = 0; i < 32; ++i)
                {
                    sprintf(hexKey + i * 2, "%02x", (int) entry->key[i]);
                }
                log_info("NATRequest", "Key:%s\n", hexKey);
                CRPNETNATRequestSend(sockfd,
                                     header->sessionID,
                                     entry->key,
                                     entry->peerUid,
                                     CRPNDR_AUDIO,
                                     header->sessionID);
                entry->messageSent = 1;
            }
            else
            {

            }
            break;
        };
    }
    return 1;
}

//主动发起NAT发现请求
int AudioRequestNATDiscover(uint32_t uid)
{
    session_id_t sessionNatDiscover = CountSessionId();//对方同意与否的处理函数session

    struct AudioDiscoverProcessEntry *entry =
            (struct AudioDiscoverProcessEntry *) calloc(1, sizeof(struct AudioDiscoverProcessEntry));
    int randNum;
    for (int randi = 0; randi < 32 / sizeof(int); ++randi)
    {
        randNum = rand();
        memcpy(entry->key + randi * sizeof(int), &randNum, sizeof(int));
    }
    entry->peerUid = uid;
    entry->messageSent = 0;
    entry->localSession = sessionNatDiscover;
    AddMessageNode(sessionNatDiscover, processNatDiscoveredOnAudio, entry);
    CRPNETNATRegisterSend(sockfd, sessionNatDiscover, entry->key);
}

//被动接收方发起NAT发现
void *AudioWaitDiscover(struct AudioDiscoverProcessEntry *entry)
{
    pthread_detach(pthread_self());
    int sockSender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int isServerDetected = 0, isRecvPipeOK = 0;
    struct sockaddr_in serverNatService;
    socklen_t serverAddrLen = sizeof(serverNatService);
    getpeername(sockfd->fd, (struct sockaddr *) &serverNatService, &serverAddrLen);
    serverNatService.sin_port = htons(8015);
    int tryTimes = 0;
    while (!entry->peerReady || !isRecvPipeOK)
    {
        log_info("RecvPipe", "%d\n", isRecvPipeOK);
        if (!isServerDetected)
        {
            sendto(sockSender, entry->peerKey, 32, 0, (struct sockaddr *) &serverNatService, serverAddrLen);
            log_info("SendKey", "To Server\n", inet_ntoa(entry->addr.sin_addr), ntohs(entry->addr.sin_port));
        }
        if (entry->addr.sin_port)//等待对方Discover
        {
            sendto(sockSender, entry->peerKey, 32, 0, (struct sockaddr *) &entry->addr, sizeof(entry->addr));
            log_info("SendKey", "To %s:%hu\n", inet_ntoa(entry->addr.sin_addr), ntohs(entry->addr.sin_port));
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockSender, &set);
        struct timeval timeout = {
                .tv_sec=1
        };
        int n;
        n = select(sockSender + 1, &set, NULL, NULL, &timeout);
        if (n > 0)
        {
            char buffer[32];
            struct sockaddr_in opaddr;
            socklen_t opAddrLen;
            n = (int) recvfrom(sockSender, buffer, 32, 0, (struct sockaddr *) &opaddr, &opAddrLen);
            if (n == 32)
            {
                if (memcmp(buffer, entry->peerKey, 32) == 0
                    || memcmp(buffer, (uint8_t[32]) {0}, 32) == 0)//与本地key相等是服务器返回数据
                {
                    isServerDetected = 1;
                    log_info("Processing", "Server Found.");
                }
                else if (memcmp(buffer, entry->key, 32) == 0)//与对点key相等,是对方发来的数据.连接已建立成功
                {
                    isRecvPipeOK = 1;
                    memcpy(&entry->addr, &opaddr, opAddrLen);
                    CRPNETNATReadySend(sockfd, entry->localSession, entry->peerUid, entry->peerSession);
                }
                else
                {
                    char hexKey[65] = {0};
                    for (int i = 0; i < 32; ++i)
                    {
                        sprintf(hexKey + i * 2, "%02x", (int) buffer[i]);
                    }
                    log_info("Key", "WrongKey%s\n", hexKey);
                }
            }
        }
        else
        {
            if (++tryTimes >= 10)//10次重试未成功
            {
                free(entry);
                return 0;
            }
        }
    }
    StartAudioChat_Send(sockSender, &entry->addr);
    free(entry);
    return NULL;
}

//接受语音聊天方
int AcceptNATDiscoverProcess(CRPBaseHeader *header, void *data)
{
    struct AudioDiscoverProcessEntry *entry = (struct AudioDiscoverProcessEntry *) data;
    switch (header->packetID)
    {
        case CRP_PACKET_FAILURE:
        {

            log_error("NatDiscover", "error\n");
            free(entry);
            return 0;
        };
        case CRP_PACKET_NET_NAT_READY:
        {
            entry->peerReady = 1;
            return 0;
        };
        case CRP_PACKET_OK:
        {
            if (!entry->messageSent)
            {
                pthread_create(&entry->workerThread, NULL, (void *(*)(void *)) AudioWaitDiscover, entry);
                CRPNETNATAcceptSend(sockfd, header->sessionID, entry->peerUid, entry->peerSession, entry->key);
                entry->messageSent = 1;
            }
            break;
        }
        case CRP_PACKET_NET_NAT_DETECTED:
        {
            CRPPacketNATDetected *packet = CRPNATDetectedCast(header);
            entry->addr.sin_family = AF_INET;
            entry->addr.sin_addr.s_addr = packet->ipv4;
            entry->addr.sin_port = packet->port;
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            return 1;
        };
    }
    return 1;
}

//收到NAT请求,同意NAT发现
int AudioAcceptNatDiscover(CRPPacketNETNATRequest *request)
{
    session_id_t sid = CountSessionId();
    struct AudioDiscoverProcessEntry *entry =
            (struct AudioDiscoverProcessEntry *) calloc(1, sizeof(struct AudioDiscoverProcessEntry));
    int randNum;
    for (int randi = 0; randi < 32 / sizeof(int); ++randi)
    {
        randNum = rand();
        memcpy(entry->key + randi * sizeof(int), &randNum, sizeof(int));
    }
    memcpy(entry->peerKey, request->key, sizeof(entry->peerKey));
    entry->peerUid = request->uid;
    entry->peerKeySet = 1;
    entry->peerSession = request->session;
    entry->localSession = sid;
    AddMessageNode(sid, AcceptNATDiscoverProcess, entry);

    char hexKey[65] = {0};
    for (int i = 0; i < 32; ++i)
    {
        sprintf(hexKey + i * 2, "%02x", (int) entry->key[i]);
    }
    log_info("Discover", "Try to register key %s\n", hexKey);
    CRPNETNATRegisterSend(sockfd, sid, entry->key);
    return 1;
}

//处理发送视频请求后，对方是否同意的函数
int deal_video_feedback(CRPBaseHeader *header, u_int32_t uid)
{
    if (header->packetID == CRP_PACKET_NET_DISCOVER_REFUSE)
    {
        g_idle_add(popup_video_request_refuse, NULL);
        //the_log_request_friend_discover.uid=-1;
        //the_log_request_friend_discover.requset_reason=-1;
    }
    if (header->packetID == CRP_PACKET_NET_INET_ADDRESS)
    {
        g_print("对方接受了您的请求");
        CRPPacketNETInetAddress *info = CRPNETInetAddressCast(header);

        log_info("UID", "%u\n", info->uid);

        struct in_addr addr;
        addr.s_addr = info->ipv4;
        char *ip = inet_ntoa(addr);
        log_info("ip", "%s\n", ip);
        struct sockaddr_in *addr_opposite = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

        addr_opposite->sin_family = AF_INET;
        addr_opposite->sin_port = htons(5555);
        addr_opposite->sin_addr = addr;
        //这里运行　视频函数，需要对方ip地址
        pthread_t pthd_video;
        pthread_create(&pthd_video, NULL, primary_video, addr_opposite);
        //primary_video(2,ip);
    }
    return 0;
}


//接到视频请求后的处理函数
//同意或者拒绝
gboolean treatment_request_video_discover(gpointer user_data)
{
    CRPPacketNETFriendDiscover *video_data = (CRPPacketNETFriendDiscover *) user_data;
    //CRPPacketMessageNormal *packet = CRPMessageNormalCast(header);
    //找到这个好友
    FriendInfo *userinfo = FriendInfoHead;
    int uidfindflag = 0;
    while (userinfo)
    {
        if (userinfo->user.uid == video_data->uid)
        {
            uidfindflag = 1;
            break;
        }
        else
        {
            userinfo = userinfo->next;
        }
    }
    //如果找到这个好友
    if (uidfindflag == 1)
    {
        //打开聊天窗口或者置前聊天窗口
        if (userinfo->chartwindow == NULL)
        {
            MainChart(userinfo);
        }
        else
        {
            gtk_window_present(GTK_WINDOW(userinfo->chartwindow));
        }
        if (userinfo->chartwindow != NULL)
        {

            GtkWidget *dialog_request_video_net_discover;

            dialog_request_video_net_discover = gtk_message_dialog_new(userinfo->chartwindow, GTK_DIALOG_MODAL,
                                                                       GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
                                                                       "莫默询问您：\n您想与这位好友视频聊天吗？");
            gtk_window_set_title(GTK_WINDOW (dialog_request_video_net_discover), "Question");
            gint result = gtk_dialog_run(GTK_DIALOG (dialog_request_video_net_discover));
            g_print("the result is %d\n", result);
            if (result == -5)
            {
                //若同意对方视频请求，就打开视频程序。这里不需要对面的ip地址.先打开监听程序然后再发送同意接受包
                //primary_video(1,NULL);
                pthread_t pthd_video_recv;
                pthread_create(&pthd_video_recv,
                               NULL,
                               primary_video,
                               NULL);
                session_id_t sessionid_accept = CountSessionId();
                //AddMessageNode(sessionid_accept, deal_video_accept_feedback, NULL);
                CRPNETDiscoverAcceptSend(sockfd, sessionid_accept, video_data->uid, video_data->session);
                gtk_widget_destroy(dialog_request_video_net_discover);
            }
            else
            {
                CRPNETDiscoverRefuseSend(sockfd, CountSessionId(), video_data->uid, video_data->session);
                gtk_widget_destroy(dialog_request_video_net_discover);
            }
        }
    }
    free(user_data);
    return 0;
}
