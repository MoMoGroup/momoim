#include <protocol/base.h>
#include <protocol/CRPPackets.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <logger.h>
#include "PopupWinds.h"
#include "audio.h"

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
        return 0;
    }
    //else if(header->packetID== CRP_PACKET_NET_INET_ADDRESS){
      //  struct Audio_Request_Info *audio_request_info=(struct Audio_Request_Info*)malloc(sizeof(struct Audio_Request_Info*));
        //CRPPacketNETInetAddress *address=CRPNETInetAddressCast(header);
        /*CRPMessageNormalSend( NULL,
                             header->sessionID,
                             UMT_AUDIO_REQUEST,
                             audio_request_info->uid,
                             0,
                             "audio_request");
        if((void*)address!=header->data){*/
          //  free(address);

    //}
}
void  audio_request_refuse(){
    the_log_request_friend_discover.uid=-1;
    the_log_request_friend_discover.requset_reason=-1;
    g_idle_add(popup_audio_request_refuse,NULL);
}

void audio_request_accept(){

}
void video_request_accept(){

}

/*int  dealwith_request_audio_net_discover(){
    if(info -> chartwindow != NULL)
    {


        GtkWidget *dialog_request_audio_net_discover;

        dialog_request_audio_net_discover = gtk_message_dialog_new(info->chartwindow, GTK_DIALOG_MODAL,
                                                                   GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
                                                                   "莫默询问您：\n您想接收这份文件吗？");
        gtk_window_set_title(GTK_WINDOW (dialog_request_audio_net_discover), "Question");
        gint result = gtk_dialog_run(GTK_DIALOG (dialog_request_audio_net_discover));
        g_print("the result is %d\n",result);
        if (result == -5)
        {
            CRPNETDiscoverAcceptSend(<#(CRPContext)context#>, <#(uint32_t)sessionID#>, <#(uint32_t)uid#>);
            return 0;
        }
        else
        {
            CRPNETDiscoverRefuseSend(<#(CRPContext)context#>, <#(uint32_t)sessionID#>, <#(uint32_t)uid#>);
            gtk_widget_destroy(dialog_request_audio_net_discover);
            return 0;
        }
    }
}*/
/*
void dealwith_recv_audio_request(gchar *recv_text, FriendInfo *info, int charlen){
    if(info->chartwindow !=NULL){
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(info->chartwindow, GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
                                        "莫默询问您：\n您想接受这份语音请求吗?");
        gtk_window_set_title(GTK_WINDOW (dialog), "Question");
        gint result = gtk_dialog_run(GTK_DIALOG (dialog));
        g_print("%the result is %d\n", result);
        if(result == -5){

        }else{
            gtk_widget_destroy(dialog);
        }

    }
}*/