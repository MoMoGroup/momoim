#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <ftadvanc.h>
#include "common.h"
#include <pwd.h>
#include <math.h>
#include "chartmessage.h"
#include "ChartRecord.h"
#include <sys/stat.h>
#include <logger.h>
#include "media.h"
#include "ScreenShot.h"
#include "ChartLook.h"
#include "../media/audio.h"
#include "onlylookinfo.h"

int flag_audio_close;

struct UserTextInformation UserWordInfo;
static cairo_surface_t *schartbackgroud, *surfacesend1, *surfacesend2, *surfacehead3, *surfacevoice1, *surfacevoice2, *surfacevoice3, *surfacevideo1, *surfacevideo2;
static cairo_surface_t *surfaceclose1, *surfaceclose2, *surfaceclosebut1, *surfaceclosebut2, *surfaceclosebut3;
static cairo_surface_t *surfacelook1, *surfacelook2, *surfacejietu1, *surfacejietu2, *surfacefile1, *surfacefile2, *surfaceimage1, *surfaceimage2;
static cairo_surface_t *surfacewordart1, *surfacewordart2, *surfacecolor, *surfacechartrecord;

static void create_surfaces(FriendInfo *information)
{
    schartbackgroud = ChangeThem_png("聊天背景.png");
    surfacesend1 = ChangeThem_png("发送1.png");
    surfacesend2 = ChangeThem_png("发送2.png");

    surfacevoice1 = ChangeThem_png("语音1.png");
    surfacevoice2 = ChangeThem_png("语音2.png");
    surfacevoice3 = ChangeThem_png("语音停止.png");

    surfacevideo1 = ChangeThem_png("视频1.png");
    surfacevideo2 = ChangeThem_png("视频2.png");

    surfacelook1 = ChangeThem_png("表情.png");
    surfacelook2 = ChangeThem_png("表情2.png");
    surfacejietu1 = ChangeThem_png("截图.png");
    surfacejietu2 = ChangeThem_png("截图2.png");
    surfacefile1 = ChangeThem_png("文件.png");
    surfacefile2 = ChangeThem_png("文件2.png");
    surfaceimage1 = ChangeThem_png("图片.png");
    surfaceimage2 = ChangeThem_png("图片2.png");
    surfacewordart1 = ChangeThem_png("字体.png");
    surfacewordart2 = ChangeThem_png("字体2.png");

    surfaceclose1 = ChangeThem_png("关闭1.png");
    surfaceclose2 = ChangeThem_png("关闭2.png");
    surfaceclosebut1 = ChangeThem_png("关闭按钮1.png");
    surfaceclosebut2 = ChangeThem_png("关闭按钮2.png");
    surfaceclosebut3 = ChangeThem_png("关闭按钮3.png");
    surfacecolor = ChangeThem_png("颜色.png");
    surfacechartrecord = ChangeThem_png("消息记录.png");

    static cairo_t *cr;
//    char mulu[80] = {0};
    cairo_surface_t *surface;
//    sprintf(mulu, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, information->user.uid);

    char filename[256];
    HexadecimalConversion(filename, information->user.icon);
    log_info("头像路径", "%s\n", filename);
    //加载一个图片
    surface = cairo_image_surface_create_from_png(filename);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    //创建画布
    surfacehead3 = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 72, 72);
    //创建画笔
    cr = cairo_create(surfacehead3);
    //缩放
    cairo_arc(cr, 36, 36, 36, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 76.0 / w, 76.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}


//背景的eventbox
static gint chartbackground_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

    }
    return 0;

}

//发送
//鼠标点击事件
static gint send_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
//    X = event->x;  // 取得鼠标相对于窗口的位置
//    Y = event->y;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标
    }

    return 0;
}

//发送
//鼠标抬起事件
static gint send_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {

        gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
        SendText(info);

    }
    return 0;

}

//发送
//鼠标移动事件
static gint send_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置发送按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend2); //置换图标

    return 0;
}

//设置离开组件事件
static gint send_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagesend, surfacesend1);

    return 0;
}


//键盘enter和alt+enter事件

gboolean key_value(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    guint keyvalue = event->keyval;
    if (keyvalue == GDK_KEY_Return || ((keyvalue == GDK_KEY_Alt_L || keyvalue == GDK_KEY_Alt_R) && (keyvalue == GDK_KEY_Return)))
    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

        SendText(info);
        return 1;
    }

    return 0;
}

//声音
//鼠标点击事件
static gint voice_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {   //设置语音按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标
    }

    return 0;
}


//声音
//鼠标抬起事件
static gint voice_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
//        uint8_t gid_audio;
//        int quantity_group = friends->groupCount;
//        //用来找出gid
//        int i, j = 0;
//        for (i = 0; i < quantity_group; i++)
//        {
//            for (j = 0; j < friends->groups[i].friendCount; j++)
//            {
//                if (friends->groups[i].friends[j] == info->user.uid)
//                {
//                    gid_audio = friends->groups[i].groupId;
//                    break;
//                }
//            }
//        }
        //标志位为0 表示没人在语音这时可以打开语音
        if (flag_audio_close == 0)
        {
            session_id_t sessionNatDiscover = CountSessionId();//对方同意与否的处理函数session

            struct AudioDiscoverProcessEntry *entry = (struct AudioDiscoverProcessEntry *) malloc(sizeof(struct AudioDiscoverProcessEntry));
            int randNum;
            for (int randi = 0; randi < 32 / sizeof(int); ++randi)
            {
                randNum = rand();
                memcpy(entry->key + randi * sizeof(int), &randNum, sizeof(int));
            }
            entry->uid = info->uid;
            entry->messageSent = 0;
            AddMessageNode(sessionNatDiscover, processNatDiscovered, entry);

            CRPNATDiscoverSend(sockfd, sessionNatDiscover, entry->key);
            gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice3);
            flag_audio_close = 1;
        }
        else//如果标志位不为０表示此时应该关掉语音.
        {
            flag_audio_close = 0;
            StopAudioChat();
            gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);
        }

    }
    return 0;

}

//声音
//鼠标移动事件
static gint voice_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置语音按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice2); //置换图标

    return 0;
}

//声音
//设置离开组件事件
static gint voice_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置语音按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    //判断此时是不是在语音聊天，是的话将语音聊天按钮设置成占线模式，否则普通模式
    if (flag_audio_close == 0)
    {
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice1);
    }
    else
    {
        gtk_image_set_from_surface((GtkImage *) info->imagevoice, surfacevoice3);
    }

    return 0;
}

//视频
//鼠标点击事件
static gint video_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {   //设置视频按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2); //置换图标
    }

    return 0;
}

//视频
//鼠标抬起事件
static gint video_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        uint8_t gid_video;
        int quantity_group = friends->groupCount;
        //用来找出gid
        int i, j = 0;
        for (i = 0; i < quantity_group; i++)
        {
            for (j = 0; j < friends->groups[i].friendCount; j++)
            {
                if (friends->groups[i].friends[j] == info->user.uid)
                {
                    gid_video = friends->groups[i].groupId;
                    break;
                }
            }
        }
        session_id_t session_id_video_server_feedback = CountSessionId();//SESSION用来处理请求送达与否
        session_id_t session_id_video_feedback = CountSessionId();//这个SESSION用来处理请求同意的情况
        //同一时间只允许发起一个请求
        //if(the_log_request_friend_discover.uid!=-1){
        //  g_idle_add(popup_request_num_limit, NULL);
        //}
        the_log_request_friend_discover.uid = info->user.uid;
        the_log_request_friend_discover.requset_reason = NET_DISCOVER_VIDEO;

        AddMessageNode(session_id_video_server_feedback, deal_video_dicover_server_feedback, NULL);
        AddMessageNode(session_id_video_feedback, deal_video_feedback, NULL);

        CRPNETFriendDiscoverSend(sockfd,
                                 session_id_video_server_feedback,
                                 gid_video,
                                 info->uid,
                                 CRPFDR_VEDIO,
                                 session_id_video_feedback
                                );


        gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1);
    }
    return 0;

}

//视频
//鼠标移动事件
static gint video_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo2); //置换图标
    return 0;
}

//视频
//鼠标likai事件
static gint video_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagevideo, surfacevideo1);
    return 0;
}

//下方关闭
//鼠标点击事件
static gint close_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {          //设置右下关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标

    }
    return 0;
}

//下方关闭
//鼠标抬起事件
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭
        gtk_widget_destroy(info->chartwindow);
        info->chartwindow = NULL;
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
        if (info->record_window != NULL)
        {
            gtk_widget_destroy(info->record_window);
            info->record_window = NULL;
        }

    }
    return 0;
}

//下方关闭
//鼠标移动事件
static gint close_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose2); //置换图标 //置换图标
    return 0;
}

//下方关闭
//鼠标likai事件
static gint close_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imageclose, surfaceclose1);//设置右下关闭

    return 0;
}

//右上关闭
//鼠标点击事件
static gint close_but_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {         //设置右上关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint close_but_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮
        gtk_widget_destroy(info->chartwindow);
        info->chartwindow = NULL;

        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
        if (info->record_window != NULL)
        {
            gtk_widget_destroy(info->record_window);
            info->record_window = NULL;
        }

    }
    return 0;

}

//鼠标移动事件
static gint close_but_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut3); //置换图标
    return 0;
}

//鼠标likai事件
static gint close_but_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gtk_image_set_from_surface((GtkImage *) info->imageclosebut, surfaceclosebut1);  //设置右上关闭按钮
    return 0;
}

//表情
//鼠标点击事件
static gint look_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint look_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {

        gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook1);
        if (info->look_window == NULL)
        {
            ChartLook(info, event->x_root, event->y_root - 130);
        }
        else
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
    }
    return 0;

}

//鼠标移动事件
static gint look_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook2); //置换图标
    return 0;
}

//鼠标likai事件
static gint look_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagelook, surfacelook1);
    return 0;
}

//jietu

//鼠标点击事件
static gint jietu_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置发送按钮
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu2); //置换图标
    return 0;
}

//鼠标抬起事件
static gint jietu_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu1);
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

        ScreenShot(info);
    }
    return 0;

}

//鼠标移动事件
static gint jietu_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu2); //置换图标

    return 0;
}

//鼠标likai事件
static gint jietu_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagejietu, surfacejietu1);

    return 0;
}

//文件
//鼠标点击事件
static gint file_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint file_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

        gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile1);
        GtkWidget *dialog;
        gchar *filename;
        dialog = gtk_file_chooser_dialog_new("Open File(s) ...", GTK_WINDOW(info->chartwindow),
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             "_Cancel", GTK_RESPONSE_CANCEL,
                                             "_Open", GTK_RESPONSE_ACCEPT,
                                             NULL);
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        while (result == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            struct stat buf;
            stat(filename, &buf);
            if ((buf.st_size / 1048576.0) < 150)
            {
                gtk_widget_destroy(dialog);
                UploadingFile(filename, info);
                break;
            }
            else
            {
                GtkWidget *cue_dialog;
                cue_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog), GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                    "文件大小不应超过150M，请选择其他文件");
                gtk_window_set_title(GTK_WINDOW (cue_dialog), "Information");
                gtk_dialog_run(GTK_DIALOG (cue_dialog));
                gtk_widget_destroy(cue_dialog);
                result = gtk_dialog_run(GTK_DIALOG(dialog));
            }
        }
        if (result == GTK_RESPONSE_CANCEL)
        {
            gtk_widget_destroy(dialog);
        }
    }
    return 0;

}

//鼠标移动事件
static gint file_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile2); //置换图标
    return 0;
}

//鼠标likai事件
static gint file_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagefile, surfacefile1);

    return 0;
}

//tupian
//鼠标点击事件
static gint photo_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint photo_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

        gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage1);
        GtkWidget *dialog;
        gchar *filename;
        dialog = gtk_file_chooser_dialog_new("Open Image(s) ...", (GtkWindow *) info->chartwindow,
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                             NULL);
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        if (result == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            GtkWidget *image;
            image = gtk_image_new_from_file(filename);

            // GtkTextBuffer *buffer;
            GtkTextMark *mark;
            GtkTextIter iter, end;
            GtkTextChildAnchor *anchor;

            size_t filenamelen;
            mark = gtk_text_buffer_get_insert(info->input_buffer);
            gtk_text_buffer_get_iter_at_mark(info->input_buffer, &iter, mark);
            anchor = gtk_text_buffer_create_child_anchor(info->input_buffer, &iter); //添加衍生构件
            filenamelen = strlen(filename);

            char *pSrc = malloc(filenamelen + 1);
            memcpy(pSrc, filename, filenamelen);
            pSrc[filenamelen] = 0;
            g_object_set_data_full(G_OBJECT(image), "ImageSrc", pSrc, free); //将路径存成为key值在image控件中保存
            gtk_widget_show_all(image);
            gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW (info->input_text), image, anchor);
            gtk_widget_grab_focus(info->input_text);
            gtk_text_buffer_get_end_iter(info->input_buffer, &end);
            GtkTextMark *text_mark_log = gtk_text_buffer_create_mark(info->input_buffer, NULL, &iter, 1);
            gtk_text_buffer_move_mark(info->input_buffer, text_mark_log, &end);
            gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(info->input_text), text_mark_log, 0, 1, 1, 1);
        }
        gtk_widget_destroy(dialog);
    }
    return 0;

}

//鼠标移动事件
static gint photo_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage2); //置换图标

    return 0;
}

//鼠标likai事件
static gint photo_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagephoto, surfaceimage1);
    return 0;
}

//将字体和颜色写入文件保存
void handle_font_color(FriendInfo *info)
{
    int num;
    UserWordInfo.coding_font_color = (gchar *) malloc(strlen(UserWordInfo.font) + 30);
    CodingWordColor(info, UserWordInfo.coding_font_color, &UserWordInfo.codinglen);
    FILE *fp;
    char wordfile[256];
    sprintf(wordfile, "%s/.momo/%u/setting", getpwuid(getuid())->pw_dir, CurrentUserInfo->uid);
    fp = fopen(wordfile, "w");
    num = fwrite(UserWordInfo.coding_font_color, 1, UserWordInfo.codinglen, fp);
    if (num == UserWordInfo.codinglen)
    {
        g_print("the wordtype write success");
    }
    fclose(fp);
}

//ziti
//鼠标点击事件
static gint wordart_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint wordart_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
        gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart1);
        GtkWidget *dialog;
        dialog = gtk_font_chooser_dialog_new("choose a font", GTK_WINDOW(event->window));

        if (UserWordInfo.description != NULL)
        {
            gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(dialog), UserWordInfo.description);
        }
        gtk_widget_show_all(dialog);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        switch (response)
        {
            case (GTK_RESPONSE_APPLY):
            case (GTK_RESPONSE_OK):
            {
                PangoFontFamily *fontFamily;
                UserWordInfo.codinglen = 0;
                fontFamily = gtk_font_chooser_get_font_family(GTK_FONT_CHOOSER(dialog));
                UserWordInfo.description = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(dialog));
                UserWordInfo.size = gtk_font_chooser_get_font_size(GTK_FONT_CHOOSER(dialog)); //大小
                UserWordInfo.size = UserWordInfo.size / 1024;
                UserWordInfo.style = pango_font_description_get_style(UserWordInfo.description); //斜体
                UserWordInfo.weight = pango_font_description_get_weight(UserWordInfo.description); //宽度
                gtk_widget_override_font(info->input_text, UserWordInfo.description); //设置输入框的字体
                UserWordInfo.font = (gchar *) pango_font_family_get_name(fontFamily);
                handle_font_color(info);
                break;
            }
            default:
                gtk_widget_destroy(GTK_WIDGET (dialog));
        }
        if (response == GTK_RESPONSE_OK)
        {
            gtk_widget_destroy(GTK_WIDGET (dialog));
        }
    }
    return 0;

}

//鼠标移动事件
static gint wordart_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

                                       gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart2); //置换图标
    return 0;
}


//鼠标likai事件
static gint wordart_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

                                       gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->imagewordart, surfacewordart1);
    return 0;
}


//颜色


//鼠标点击事件
static gint color_button_press_event(GtkWidget *widget,

                                     GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//鼠标抬起事件
static gint color_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
        GtkColorSelectionDialog *dialog;
        GtkColorSelection *colorsel;
        GdkColor color;
        dialog = GTK_COLOR_SELECTION_DIALOG(gtk_color_selection_dialog_new("ColorSelect"));
        color.red = 0;
        color.blue = 65535;
        color.green = 0;
        colorsel = GTK_COLOR_SELECTION((gtk_color_selection_dialog_get_color_selection((dialog))));
        gtk_color_selection_set_has_opacity_control(colorsel, 1);//1原来是true
        gtk_color_selection_set_has_palette(colorsel, 1);
        gtk_color_selection_set_previous_color(colorsel, &color);
        gtk_color_selection_set_current_color(colorsel, &color);
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
        {
            gtk_color_selection_get_current_color(colorsel, &color);
            UserWordInfo.color_red = color.red;
            UserWordInfo.color_blue = color.blue;
            UserWordInfo.color_green = color.green;
            GdkRGBA rgbacolor;
            rgbacolor.alpha = 1;
            rgbacolor.red = UserWordInfo.color_red / 65535.0;
            rgbacolor.green = UserWordInfo.color_green / 65535.0;
            rgbacolor.blue = UserWordInfo.color_blue / 65535.0;
            gtk_widget_override_color(info->input_text, GTK_STATE_FLAG_NORMAL, &rgbacolor);
            handle_font_color(info);
        }

        gtk_widget_destroy(GTK_WIDGET(dialog));
    }

    return 0;

}

//鼠标移动事件
static gint color_enter_notify_event(GtkWidget *widget, GdkEventButton *event,

                                     gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}


//鼠标likai事件
static gint color_leave_notify_event(GtkWidget *widget, GdkEventButton *event,

                                     gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    return 0;
}


////颜色点击事件
//static gint color_click_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
//{
//
//}
//聊天记录
//鼠标点击事件
static gint chartrecord_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {   //设置视频按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }

    return 0;
}

//聊天记录
//鼠标抬起事件
static gint chartrecord_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        if (info->look_window != NULL)
        {
            gtk_widget_destroy(info->look_window);
            info->look_window = NULL;
        }

        if (info->record_window == NULL)
        {
            ChartRecord(info);
        }
        else
        {
            gtk_window_present(GTK_WINDOW(info->record_window));
        }

    }
    return 0;

}

//聊天记录
//鼠标移动事件
static gint chartrecord_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//聊天记录
//鼠标likai事件
static gint chartrecord_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->chartwindow), gdk_cursor_new(GDK_ARROW));
    return 0;
}

static gint text_view_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (info->look_window != NULL)
    {
        gtk_widget_destroy(info->look_window);
        info->look_window = NULL;
    }
    return 0;
}

static gint nicheng_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (info->Infowind == NULL)
    {
        OnlyLookInfo(info);
    }
    else
    {
        gtk_window_present(GTK_WINDOW(info->Infowind));
    }
    return 0;
}


int MainChart(FriendInfo *friendinfonode)
{

    GtkEventBox *chartbackground_event_box, *send_event_box, *voice_event_box, *video_event_box;
    GtkEventBox *close_event_box, *close_but_event_box, *look_event_box, *jietu_event_box, *file_event_box;
    GtkEventBox *photo_event_box, *wordart_event_box, *color_event_box, *chartrecord_event_box, *nicheng_event_box;

    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    friendinfonode->chartwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    friendinfonode->chartlayout = gtk_fixed_new();
    friendinfonode->chartlayout2 = gtk_layout_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER (friendinfonode->chartwindow),
                      friendinfonode->chartlayout2);//chartlayout2 加入到window
    gtk_container_add(GTK_CONTAINER (friendinfonode->chartlayout2), friendinfonode->chartlayout);

    g_signal_connect(G_OBJECT(friendinfonode->chartwindow), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_default_size(GTK_WINDOW(friendinfonode->chartwindow), 500, 550);
    gtk_window_set_position(GTK_WINDOW(friendinfonode->chartwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    //gtk_window_set_resizable (GTK_WINDOW (window), FALSE);//窗口不可改变

    gtk_window_set_decorated(GTK_WINDOW(friendinfonode->chartwindow), FALSE);   // 去掉边框

    create_surfaces(friendinfonode);

    friendinfonode->imageflowerbackgroud = gtk_image_new_from_surface(schartbackgroud);
    friendinfonode->imagesend = gtk_image_new_from_surface(surfacesend1);
    friendinfonode->imagevoice = gtk_image_new_from_surface(surfacevoice1);
    friendinfonode->imagevideo = gtk_image_new_from_surface(surfacevideo1);
    friendinfonode->imageclose = gtk_image_new_from_surface(surfaceclose1);
    friendinfonode->imageclosebut = gtk_image_new_from_surface(surfaceclosebut1);
    friendinfonode->imagelook = gtk_image_new_from_surface(surfacelook1);
    friendinfonode->imagejietu = gtk_image_new_from_surface(surfacejietu1);
    friendinfonode->imagefile = gtk_image_new_from_surface(surfacefile1);
    friendinfonode->imagephoto = gtk_image_new_from_surface(surfaceimage1);
    friendinfonode->imagewordart = gtk_image_new_from_surface(surfacewordart1);
    friendinfonode->imagecolor = gtk_image_new_from_surface(surfacecolor);
    friendinfonode->imagechartrecord = gtk_image_new_from_surface(surfacechartrecord);

// 设置窗体获取鼠标事件 背景

    chartbackground_event_box = BuildEventBox(
            friendinfonode->imageflowerbackgroud,
            G_CALLBACK(chartbackground_button_press_event),
            NULL, NULL, NULL, NULL, friendinfonode);

    //发送

    send_event_box = BuildEventBox(
            friendinfonode->imagesend,
            G_CALLBACK(send_button_press_event),
            G_CALLBACK(send_enter_notify_event),
            G_CALLBACK(send_leave_notify_event),
            G_CALLBACK(send_button_release_event),
            NULL,
            friendinfonode);

    //语音


    voice_event_box = BuildEventBox(
            friendinfonode->imagevoice,
            G_CALLBACK(voice_button_press_event),
            G_CALLBACK(voice_enter_notify_event),
            G_CALLBACK(voice_leave_notify_event),
            G_CALLBACK(voice_button_release_event),
            NULL,
            friendinfonode);
    //视频

    video_event_box = BuildEventBox(
            friendinfonode->imagevideo,
            G_CALLBACK(video_button_press_event),
            G_CALLBACK(video_enter_notify_event),
            G_CALLBACK(video_leave_notify_event),
            G_CALLBACK(video_button_release_event),
            NULL,
            friendinfonode);
//下方关闭按钮

    close_event_box = BuildEventBox(
            friendinfonode->imageclose,
            G_CALLBACK(close_button_press_event),
            G_CALLBACK(close_enter_notify_event),
            G_CALLBACK(close_leave_notify_event),
            G_CALLBACK(close_button_release_event),
            NULL,
            friendinfonode);

//右上方关闭按钮
    close_but_event_box = BuildEventBox(
            friendinfonode->imageclosebut,
            G_CALLBACK(close_but_button_press_event),
            G_CALLBACK(close_but_enter_notify_event),
            G_CALLBACK(close_but_leave_notify_event),
            G_CALLBACK(close_but_button_release_event),
            NULL,
            friendinfonode);
//表情

    look_event_box = BuildEventBox(
            friendinfonode->imagelook,
            G_CALLBACK(look_button_press_event),
            G_CALLBACK(look_enter_notify_event),
            G_CALLBACK(look_leave_notify_event),
            G_CALLBACK(look_button_release_event),
            NULL,
            friendinfonode);

    //截图

    jietu_event_box = BuildEventBox(
            friendinfonode->imagejietu,
            G_CALLBACK(jietu_button_press_event),
            G_CALLBACK(jietu_enter_notify_event),
            G_CALLBACK(jietu_leave_notify_event),
            G_CALLBACK(jietu_button_release_event),
            NULL,
            friendinfonode);
//文件

    file_event_box = BuildEventBox(
            friendinfonode->imagefile,
            G_CALLBACK(file_button_press_event),
            G_CALLBACK(file_enter_notify_event),
            G_CALLBACK(file_leave_notify_event),
            G_CALLBACK(file_button_release_event),
            NULL,
            friendinfonode);

    //图片

    photo_event_box = BuildEventBox(
            friendinfonode->imagephoto,
            G_CALLBACK(photo_button_press_event),
            G_CALLBACK(photo_enter_notify_event),
            G_CALLBACK(photo_leave_notify_event),
            G_CALLBACK(photo_button_release_event),
            NULL,
            friendinfonode);
    //字体

    wordart_event_box = BuildEventBox(
            friendinfonode->imagewordart,
            G_CALLBACK(wordart_button_press_event),
            G_CALLBACK(wordart_enter_notify_event),
            G_CALLBACK(wordart_leave_notify_event),
            G_CALLBACK(wordart_button_release_event),
            NULL,
            friendinfonode);
//颜色
    color_event_box = BuildEventBox(
            friendinfonode->imagecolor,
            G_CALLBACK(color_button_press_event),
            G_CALLBACK(color_enter_notify_event),
            G_CALLBACK(color_leave_notify_event),
            G_CALLBACK(color_button_release_event),
            NULL,
            friendinfonode);
//聊天记录
    chartrecord_event_box = BuildEventBox(
            friendinfonode->imagechartrecord,
            G_CALLBACK(chartrecord_button_press_event),
            G_CALLBACK(chartrecord_enter_notify_event),
            G_CALLBACK(chartrecord_leave_notify_event),
            G_CALLBACK(chartrecord_button_release_event),
            NULL,
            friendinfonode);


//背景
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(chartbackground_event_box), 0, 0);
    //逆臣  //daxiao
    GtkWidget *nicheng;
    PangoFontDescription *font;
    nicheng = gtk_label_new(friendinfonode->user.nickName);

    font = pango_font_description_from_string("Droid Sans Mono");//"Droid Sans Mono"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(nicheng, font);
    nicheng_event_box = BuildEventBox(
            nicheng,
            G_CALLBACK(nicheng_button_event),
            NULL, NULL, NULL, NULL, friendinfonode);
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), nicheng_event_box, 100, 20);
//发送
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(send_event_box), 390, 512);
//语音
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(voice_event_box), 80, 50);
//视频按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(video_event_box), 120, 50);
//下方关闭按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(close_event_box), 300, 514);
//右上角关闭按钮
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(close_but_event_box), 460, 0);
//表情
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(look_event_box), 80, 405);
//截图
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(jietu_event_box), 210, 412);
//文件
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(file_event_box), 165, 405);
//图片
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(photo_event_box), 120, 405);
//字体
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(wordart_event_box), 5, 405);
    //颜色
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(color_event_box), 44, 410);
    //聊天记录
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(chartrecord_event_box), 420, 410);
    //
//头像
    friendinfonode->imagehead3 = gtk_image_new_from_surface(surfacehead3);
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), friendinfonode->imagehead3, 15, 8);

    //创建发送文本框，和接受文本框
    friendinfonode->input_text = gtk_text_view_new();
    friendinfonode->show_text = gtk_text_view_new();
    g_signal_connect(friendinfonode->input_text,
                     "button_press_event",
                     G_CALLBACK(text_view_button_event),
                     friendinfonode);
    g_signal_connect(friendinfonode->show_text,
                     "button_press_event",
                     G_CALLBACK(text_view_button_event),
                     friendinfonode);

    friendinfonode->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->input_text));
    friendinfonode->show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (friendinfonode->show_text));

    g_signal_connect(friendinfonode->input_text, "key-press-event", G_CALLBACK(key_value), friendinfonode);

    //创建文字标记
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "red_foreground", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "gray_foreground", "foreground", "gray", NULL);
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "blue_foreground", "foreground", "blue", NULL);
    gtk_text_buffer_create_tag(friendinfonode->show_buffer, "size1", "font", "12", NULL);
    //创建滚动窗口
    friendinfonode->sw1 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    friendinfonode->sw2 = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_container_add(GTK_CONTAINER(friendinfonode->sw1), friendinfonode->input_text);
    gtk_container_add(GTK_CONTAINER(friendinfonode->sw2), friendinfonode->show_text);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->input_text), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(friendinfonode->show_text), GTK_WRAP_WORD_CHAR);//自动换行

    gtk_text_view_set_editable(GTK_TEXT_VIEW(friendinfonode->show_text), 0);//不可编辑
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(friendinfonode->show_text), FALSE);

    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw1), 2, 438);//文本框位置
    gtk_fixed_put(GTK_FIXED(friendinfonode->chartlayout), GTK_WIDGET(friendinfonode->sw2), 3, 89);

    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw1), 495, 75);
    gtk_widget_set_size_request(GTK_WIDGET(friendinfonode->sw2), 495, 320);//大小

    GdkRGBA rgba = {0.92, 0.88, 0.74, 1};
    gtk_widget_override_background_color(friendinfonode->input_text, GTK_STATE_FLAG_NORMAL, &rgba);//设置透明
    gtk_widget_override_background_color(friendinfonode->show_text, GTK_STATE_FLAG_NORMAL, &rgba);//设置透明

//    comboBox = gtk_combo_box_new();
//    gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), 0);
    //设置打开窗口后字体样式

    gtk_widget_override_font(friendinfonode->input_text, UserWordInfo.description);

    GdkRGBA rgbacolor;
    rgbacolor.alpha = 1;
    rgbacolor.red = UserWordInfo.color_red / 65535.0;
    rgbacolor.green = UserWordInfo.color_green / 65535.0;
    rgbacolor.blue = UserWordInfo.color_blue / 65535.0;
    gtk_widget_override_color(friendinfonode->input_text, GTK_STATE_FLAG_NORMAL, &rgbacolor);
    gtk_widget_show_all(friendinfonode->chartwindow);


    // gtk_main();

    return 0;
}