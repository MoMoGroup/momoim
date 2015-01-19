#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <logger.h>
#include <common.h>
#include <arpa/inet.h>
#include "PopupWinds.h"
#include "media.h"
#include "ClientSockfd.h"
#include "MainInterface.h"
//#include "../media/sound.h"
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


//处理发送音频请求后，对方是否同意的函数
int processNatDiscovered(CRPBaseHeader *header, void *data)
{
    struct AudioDiscoverProcessEntry *entry = (struct AudioDiscoverProcessEntry *) data;
    if (header->packetID == CRP_PACKET_NET_DETECTED)
    {
        CRPPacketNATDetected *packet = CRPNATDetectedCast(header);
        StartAudioChat_Send(&packet->addr);

        if ((void *) packet != header->data)
        {
            free(packet);
        }
        return 0;
    }
    if (header->packetID == CRP_PACKET_OK)
    {
        if (!entry->messageSent)
        {
            char hexKey[65] = {0};
            for (int i = 0; i < 32; ++i)
            {
                sprintf(hexKey + i * 2, "%02x", entry->key[i]);
            }
            log_info("NatDiscover", "Key:%s\n", hexKey);
            CRPMessageNormalSend(sockfd, header->sessionID, UMT_NAT_REQUEST, entry->uid, 32, entry->key);
            entry->messageSent = 1;
        }
        else
        {

        }
    }
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
        pthread_create(&pthd_video,
                       NULL,
                       primary_video,
                       addr_opposite);
        //primary_video(2,ip);
    }
    return 0;
}




//处理服务器发送accept_net_friend_discover的函数
//包括　发送accept失败，或者发送accept成功，成功的话运行音频程序
//int deal_audio_accept_feedback(CRPBaseHeader *header){
//    if(header->packetID==CRP_PACKET_FAILURE) {
//        g_idle_add(popup_audio, NULL);
//        the_log_request_friend_discover.uid=-1;
//        the_log_request_friend_discover.requset_reason=-1;
//    }else{
//        //被动方开始运行音频程序，不需要对方的ip地址
////        AudioChatRoutine();
//    }
//    return 0;
//}





////处理服务器发送accept_net_friend_discover的函数
////包括　发送accept失败，或者发送accept成功，成功的话运行视频程序
//int deal_video_accept_feedback(CRPBaseHeader *header){
//    if(header->packetID==CRP_PACKET_FAILURE) {
//        g_idle_add(popup_audio, NULL);
//        the_log_request_friend_discover.uid=-1;
//        the_log_request_friend_discover.requset_reason=-1;
//    }else{
//        //被动方进行视频程序 ，不需要需要对方的ip地址
//        primary_video(1,NULL);
//    }
//    return 0;
//}


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
