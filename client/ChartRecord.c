#include <gtk/gtk.h>
#include <common.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "ChartRecord.h"
#include <protocol/CRPPackets.h>
#include <ftlist.h>
#include <logger.h>
#include <imcommon/user.h>
#include <protocol/base.h>
#include <protocol/message/RecordData.h>
#include <tttables.h>
#include <imcommon/message.h>

static cairo_surface_t *surface_record_background, *surface_next, *surfaceclosebut1, *surfaceclosebut2, *surfaceclosebut3;
static cairo_surface_t *surface_calendar;

static void create_surfaces()
{
    if (surface_record_background == NULL)
    {
        surface_record_background = ChangeThem_png("消息记录界面.png");
        surface_next = ChangeThem_png("翻页.png");
        surfaceclosebut1 = ChangeThem_png("关闭按钮1.png");
        surfaceclosebut2 = ChangeThem_png("关闭按钮2.png");
        surfaceclosebut3 = ChangeThem_png("关闭按钮3.png");
        surface_calendar = ChangeThem_png("日历.png");
    }
}


void DecodingRecordText(const gchar *text, FriendInfo *info, int count)
{

    gchar *ptext = text, *ptext_end = text + count;
    GtkTextBuffer *show_buffer;
    GtkTextIter start, end;
    GtkTextTag *wordtag;
    show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(info->record_text));
    gtk_text_buffer_get_bounds(show_buffer, &start, &end);
    wordtag = gtk_text_buffer_create_tag(show_buffer, NULL, NULL, NULL);
    while (ptext < ptext_end)
    {
        if (*ptext == '\0')
        {
            switch (*(ptext + 1))
            {
                case  1:      //字体类型
                {
                    int i;

                    for (i = 2; ptext[i]; ++i)
                    {

                    }
                    g_object_set(wordtag, "font", ptext + 2, NULL);
                    ptext = ptext + i + 1;
                    break;

                };

                case 2: //是否斜体
                {
                    ptext = ptext + 2;
                    int style_value = *ptext;
                    if (style_value == 1)
                    {
                        g_object_set(wordtag, "style", PANGO_STYLE_ITALIC, NULL);
                    }
                    ptext++;
                    break;
                };
                case 3:   //宽度
                {
                    ptext = ptext + 2;
                    int weight_value = 0;
                    memcpy(&weight_value, ptext, 2);
                    g_object_set(wordtag, "weight", weight_value, NULL);
                    ptext = ptext + 2;
                    break;
                };
                case 4: //字体大小
                {
                    ptext = ptext + 2;
                    gint size_value;
                    size_value = *ptext;
                    g_object_set(wordtag, "size", size_value * 1024, NULL);
                    g_object_set(wordtag, "size-set", 1, NULL);
                    ptext++;
                    break;
                };
                case 5:
                {
                    ptext = ptext + 2;
                    guint16 colorred;
                    guint16 colorgreen;
                    guint16 colorblue;
                    GdkRGBA rgba;
                    memcpy(&colorred, ptext, 2);
                    memcpy(&colorgreen, ptext + 2, 2);
                    memcpy(&colorblue, ptext + 4, 2);
                    g_print("the red is %u\n", colorred);
                    g_print("the green is %u\n", colorgreen);
                    g_print("the blue is %u\n", colorblue);
                    rgba.alpha = 1;
                    rgba.red = colorred / 65535.0;
                    rgba.green = colorgreen / 65535.0;
                    rgba.blue = colorblue / 65535.0;
                    g_print("the red rgba is %lf\n", rgba.red);
                    g_print("the green rgba is %lf\n", rgba.green);
                    g_print("the blue is rgba %lf\n", rgba.blue);
                    g_object_set(wordtag, "foreground-rgba", &rgba, NULL);
                    ptext = ptext + 6;
                    break;
                }
                case 0:
                {
                    GtkTextChildAnchor *anchor;
                    GtkWidget *image;
                    char filename[256] = {0};
                    char strdest[16] = {0};
                    ptext = ptext + 2;
                    memcpy(strdest, ptext, 16);
                    HexadecimalConversion(filename, strdest); //进制转换，将MD5值的字节流转换成十六进制
                    anchor = gtk_text_buffer_create_child_anchor(show_buffer, &end);
                    image = gtk_image_new_from_file(filename);
                    gtk_widget_show_all(image);
                    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(info->record_text), image, anchor);
                    ptext = ptext + 16;
                    break;
                }
                default:
                    break;
            }


        }
        else
        {
            gchar *next_char = g_utf8_next_char(ptext);
            gtk_text_buffer_insert_with_tags(show_buffer, &end, ptext, next_char - ptext, wordtag, NULL);
            ptext = next_char;
        }

    }
    gtk_text_buffer_insert_with_tags_by_name(show_buffer, &end,
                                             "\n", -1, "gray_foreground", NULL);
//    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(info->record_text),
//                                       gtk_text_buffer_get_insert(info->show_buffer));//实现自动滚屏的效果
}

gboolean show_record_message(void *data)
{
    struct RcordMessageData *record_message_data = (struct RcordMessageData *) data;

    char nicheng_times[60] = {0};
    struct tm *p;
    GtkTextIter start, end;
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_message_data->info->record_text));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    p = localtime(&record_message_data->time);
    if (record_message_data->record_user_uid == record_message_data->info->user.uid)
    {
        sprintf(nicheng_times,
                " %s  %d: %d: %d \n",
                record_message_data->info->user.nickName,
                p->tm_hour,
                p->tm_min,
                p->tm_sec);
        gtk_text_buffer_insert_with_tags_by_name(buffer, &end,
                                                 nicheng_times, -1, "blue_foreground", "size1", NULL);
    }
    else if (record_message_data->record_user_uid == CurrentUserInfo->uid)
    {
        sprintf(nicheng_times, " %s  %d: %d: %d \n", CurrentUserInfo->nickName, p->tm_hour, p->tm_min, p->tm_sec);
        gtk_text_buffer_insert_with_tags_by_name(buffer, &end,
                                                 nicheng_times, -1, "red_foreground", "size1", NULL);
    }
    DecodingRecordText(record_message_data->record_message, record_message_data->info, record_message_data->messageLen);
    return 0;
}

int handle_record_message(CRPBaseHeader *header, void *data)
{
    FriendInfo *info = (FriendInfo *) data;
    size_t num;
    int ret = 1;
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        CRPPacketFailure *infodata = CRPFailureCast(header);
        log_info("FAILURE reason", infodata->reason);
        return 0;
    }
    log_info("Message", "Packet id :%d,SessionID:%d\n", header->packetID, header->sessionID);

    if (header->packetID == CRP_PACKET_OK)
    {
        if (info->record_end_flag == 0)
        {
            info->record_end_flag = 1;
        }
        else
        {
            return 0;
        }

    }
    if (header->packetID == CRP_PACKET_MESSAGE_RECORD_DATA)
    {
        struct RcordMessageData *record_message_data = (struct RcordMessageData *) malloc(sizeof(struct RcordMessageData));
        record_message_data->info = info;
        CRPPacketMessageRecordData *packet = CRPMessageRecordDataCast(header);
        record_message_data->id = packet->messages.id;
        record_message_data->record_user_uid = packet->messages.from;
        record_message_data->messageLen = packet->messages.messageLen;
        record_message_data->time = packet->messages.time;
        record_message_data->messageType = packet->messages.messageType;
        record_message_data->record_message = (gchar *) malloc(packet->messages.messageLen);
        memcpy(record_message_data->record_message, packet->messages.content, packet->messages.messageLen);
        g_idle_add(show_record_message, record_message_data);
    }


    return ret;
}

void get_record(FriendInfo *info, MessageQueryCondition *message_query_conditon)
{
    info->record_end_flag = 0;
    CRPMessageRecordQuerySend(sockfd, info->session_id, message_query_conditon);
    free(message_query_conditon);
}


//背景的eventbox
static gint record_background_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        if (info->calendar != NULL)
        {
            gtk_widget_destroy(info->calendar);
            info->calendar = NULL;
        }

        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;

}

//翻页
//鼠标点击事件
static gint record_next_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }

    return 0;
}

//
//鼠标抬起事件
static gint record_next_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {

    }
    return 0;

}

//
//鼠标移动事件
static gint record_next_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//设置离开组件事件
static gint record_next_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

static void calendar_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    struct tm ptime;
    time_t times;
    char date[40] = {0};
    guint year;
    guint month;
    guint day;
    gtk_calendar_get_date(GTK_CALENDAR(info->calendar), &year, &month, &day);/*取得选择的年月日*/
    ptime.tm_year = year;
    ptime.tm_mon = month;
    ptime.tm_mday = day;
    times = mktime(&ptime);
    sprintf(date, "  %d / %d / %d \n", year, 1 + month, day);
    gtk_label_set_text(GTK_LABEL(info->record_date), date);
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 12 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(info->record_date, font);
    gtk_fixed_put(GTK_FIXED(info->record_layout2), info->record_date, 30, 60);
    g_print("%d %d %d", year, month + 1, day);
    gtk_widget_destroy(widget);
    widget = NULL;
    get_record(info, times);

}

//日历
//鼠标点击事件
static gint calendar_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//鼠标抬起事件
static gint calendar_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        info->calendar = gtk_calendar_new();
        gtk_widget_show(info->calendar);
        gtk_fixed_put(GTK_FIXED(info->record_layout2), info->calendar, 20, 60);
        g_signal_connect(info->calendar, "day-selected-double-click", G_CALLBACK(calendar_event), info);
    }
    return 0;

}

//
//鼠标移动事件
static gint calendar_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//设置离开组件事件
static gint calendar_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

//右上关闭
//鼠标点击事件
static gint close_but_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (event->button == 1)
    {         //设置右上关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->image_record_close, surfaceclosebut2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint close_but_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_ARROW));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) info->image_record_close, surfaceclosebut1);  //设置右上关闭按钮
        gtk_widget_destroy(info->record_window);
        info->record_window = NULL;
    }
    return 0;

}

//鼠标移动事件
static gint close_but_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) info->image_record_close, surfaceclosebut3); //置换图标
    return 0;
}

//鼠标likai事件
static gint close_but_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    gdk_window_set_cursor(gtk_widget_get_window(info->record_window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) info->image_record_close, surfaceclosebut1);  //设置右上关闭按钮
    return 0;
}

void ChartRecord(FriendInfo *info)
{
    GtkEventBox *record_background_event_box, *record_next_event_box, *record_close_event_box, *calendar_event_box;
    GtkWidget *image_calendar, *image_record_background;

    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    info->record_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    info->record_layout = gtk_layout_new(NULL, NULL);
    info->record_layout2 = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER (info->record_window), info->record_layout);//chartlayout 加入到window
    gtk_container_add(GTK_CONTAINER (info->record_layout), info->record_layout2);

    g_signal_connect(G_OBJECT(info->record_window), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(info->record_window), 318, 550);
    gtk_window_set_position(GTK_WINDOW(info->record_window), GTK_WIN_POS_MOUSE);//窗口出现位置


    gtk_window_set_decorated(GTK_WINDOW(info->record_window), FALSE);   // 去掉边框
    create_surfaces();
    image_record_background = gtk_image_new_from_surface(surface_record_background);
    info->image_record_next = gtk_image_new_from_surface(surface_next);
    info->image_record_close = gtk_image_new_from_surface(surfaceclosebut1);
    image_calendar = gtk_image_new_from_surface(surface_calendar);
// 设置窗体获取鼠标事件 背景

    record_background_event_box = BuildEventBox(
            image_record_background,
            G_CALLBACK(record_background_button_press_event),
            NULL, NULL, NULL, NULL, info);

    //翻页
    record_next_event_box = BuildEventBox(
            info->image_record_next,
            G_CALLBACK(record_next_button_press_event),
            G_CALLBACK(record_next_enter_notify_event),
            G_CALLBACK(record_next_leave_notify_event),
            G_CALLBACK(record_next_button_release_event),
            NULL,
            info);
    calendar_event_box = BuildEventBox(
            image_calendar,
            G_CALLBACK(calendar_button_press_event),
            G_CALLBACK(calendar_enter_notify_event),
            G_CALLBACK(calendar_leave_notify_event),
            G_CALLBACK(calendar_button_release_event),
            NULL,
            info);
    record_close_event_box = BuildEventBox(
            info->image_record_close,
            G_CALLBACK(close_but_button_press_event),
            G_CALLBACK(close_but_enter_notify_event),
            G_CALLBACK(close_but_leave_notify_event),
            G_CALLBACK(close_but_button_release_event),
            NULL,
            info);
    //背景
    gtk_fixed_put(GTK_FIXED(info->record_layout2), record_background_event_box, 0, 0);
    //聊天记录翻页
    gtk_fixed_put(GTK_FIXED(info->record_layout2), record_next_event_box, 280, 510);
    //右上角关闭按钮
    gtk_fixed_put(GTK_FIXED(info->record_layout2), record_close_event_box, 280, 0);

    //日历
    gtk_fixed_put(GTK_FIXED(info->record_layout2), calendar_event_box, 2, 60);

    //创建文本框

    info->record_text = gtk_text_view_new();
    info->record_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (info->record_text));

    //创建文字标记
    gtk_text_buffer_create_tag(info->record_buffer, "red_foreground", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(info->record_buffer, "gray_foreground", "foreground", "gray", NULL);
    gtk_text_buffer_create_tag(info->record_buffer, "blue_foreground", "foreground", "blue", NULL);
    gtk_text_buffer_create_tag(info->record_buffer, "size1", "font", "12", NULL);
    //创建滚动窗口
    info->record_sw = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_container_add(GTK_CONTAINER(info->record_sw), info->record_text);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(info->record_text), GTK_WRAP_WORD_CHAR);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(info->record_text), 0);//不可编辑
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(info->record_text), FALSE);

    gtk_fixed_put(GTK_FIXED(info->record_layout2), GTK_WIDGET(info->record_sw), 2, 100);//文本框位置

    gtk_widget_set_size_request(GTK_WIDGET(info->record_sw), 315, 380);
    GdkRGBA rgba = {0.92, 0.88, 0.74, 1};
    gtk_widget_override_background_color(info->record_text, GTK_STATE_FLAG_NORMAL, &rgba);//设置透明

//获取当前时间
    char date[40] = {0};
    time_t date_time;
    time(&date_time);
    struct tm *p;
    p = localtime(&date_time);
    sprintf(date, "  %d / %d / %d \n", (1900 + p->tm_year), 1 + p->tm_mon, p->tm_mday);
    info->record_date = gtk_label_new(date);
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 12 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(info->record_date, font);
    gtk_fixed_put(GTK_FIXED(info->record_layout2), info->record_date, 30, 60);
    info->session_id = CountSessionId();
    AddMessageNode(info->session_id, handle_record_message, info);

    //当刚打开聊天记录时初始化
    MessageQueryCondition *message_query_conditon = (MessageQueryCondition *) malloc(sizeof(MessageQueryCondition));
    message_query_conditon->from = info->user.uid;
    message_query_conditon->to = info->user.uid;
    message_query_conditon->time = date_time;
    message_query_conditon->timeOperator = -1;
    message_query_conditon->limit = 20;
    message_query_conditon->messageType = UMT_TEXT;
    get_record(info, message_query_conditon);
    gtk_widget_show_all(info->record_window);
}