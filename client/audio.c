#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <logger.h>
#include <common.h>
#include "PopupWinds.h"
#include "audio.h"
#include "ClientSockfd.h"
#include "MainInterface.h"
//#include "../media/sound.h"

//这个结构体用来保存请求音视频的记录
struct log_request_friend_discover the_log_request_friend_discover;

//服务器送达数据包错误时弹窗
static int popup_audio(gpointer p){
    popup("错误","无法建立连接");
    return 0;
}
//对方拒绝请求时的弹窗
static int popup_audio_request_refuse(gpointer p){
    popup("消息","对方已拒绝您的请求");
    return 0;
}
//对方同意请求时的弹窗
static int popup_audio_request_accept(gpointer p){
    popup("消息","对方已接受了您的请求");
    return 0;
}
//提示弹窗
int popup_request_num_limit(gpointer p){
    popup("消息","同一时间只能对一个好友发起请求哦");
    return 0;
}

//处理服务器送达net_friend_discover的函数
int deal_dicover_send_feedback(CRPBaseHeader *header, u_int32_t uid){
    if(header->packetID==CRP_PACKET_FAILURE) {
        g_idle_add(popup_audio, NULL);
        the_log_request_friend_discover.uid=-1;
        the_log_request_friend_discover.requset_reason=-1;
    }
    return 0;
}

//处理服务器送达accept_net_friend_discover的函数
int deal_audio_dicover_accept_feedback(CRPBaseHeader *header){
    if(header->packetID==CRP_PACKET_FAILURE) {
        g_idle_add(popup_audio, NULL);
        the_log_request_friend_discover.uid=-1;
        the_log_request_friend_discover.requset_reason=-1;
    }else{
//        primary_audio();
    }
    return 0;
}

//处理服务器送达accept_net_friend_discover的函数
int deal_video_dicover_accept_feedback(CRPBaseHeader *header){
    if(header->packetID==CRP_PACKET_FAILURE) {
        g_idle_add(popup_audio, NULL);
        the_log_request_friend_discover.uid=-1;
        the_log_request_friend_discover.requset_reason=-1;
    }else{
//        primary_audio();
    }
    return 0;
}


int   audio_request_refuse(){
    the_log_request_friend_discover.uid=-1;
    the_log_request_friend_discover.requset_reason=-1;
    g_idle_add(popup_audio_request_refuse,NULL);
    return 0;
}

int audio_request_accept(){
    g_idle_add(popup_audio_request_accept,NULL);
    return 0;
}
int video_request_accept(){
    return 0;
}

gboolean treatment_request_video_net_discover(gpointer user_data)
{
    CRPPacketNETFriendDiscover *video_data = (CRPPacketNETFriendDiscover *)user_data;
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
        if(userinfo -> chartwindow != NULL)
        {

            GtkWidget *dialog_request_video_net_discover;

            dialog_request_video_net_discover = gtk_message_dialog_new(userinfo->chartwindow, GTK_DIALOG_MODAL,
                                                                       GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
                                                                       "莫默询问您：\n您想与这位好友视频聊天吗？");
            gtk_window_set_title(GTK_WINDOW (dialog_request_video_net_discover), "Question");
            gint result = gtk_dialog_run(GTK_DIALOG (dialog_request_video_net_discover));
            g_print("the result is %d\n",result);
            if (result == -5)
            {
                session_id_t sessionid_accept=CountSessionId();
                AddMessageNode(sessionid_accept, deal_video_dicover_accept_feedback, NULL);
                CRPNETDiscoverAcceptSend(sockfd , sessionid_accept, video_data->uid,sessionid_accept);
                gtk_widget_destroy(dialog_request_video_net_discover);
            }
            else
            {
                CRPNETDiscoverRefuseSend(sockfd , CountSessionId(), video_data->uid, CountSessionId());
                gtk_widget_destroy(dialog_request_video_net_discover);
            }
        }
    }
    free(user_data);
    return 0;
}

