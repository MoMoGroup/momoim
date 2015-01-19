#include <gtk/gtk.h>
#include <common.h>
#include <stdlib.h>
#include "ClientSockfd.h"
#include "MainInterface.h"
#include "ChartRecord.h"
#include <protocol/CRPPackets.h>
#include <ftlist.h>
#include <logger.h>
#include <protocol/message/RecordData.h>


static cairo_surface_t *surface_record_background, *surface_next, *surfaceclosebut1, *surfaceclosebut2, *surfaceclosebut3;
static cairo_surface_t *surface_upward, *surface_calendar;

static void create_surfaces()
{
    if (surface_record_background == NULL)
    {
        surface_record_background = ChangeThem_png("消息记录界面.png");
        surface_next = ChangeThem_png("翻页.png");
        surface_upward = ChangeThem_png("上翻页.png");
        surfaceclosebut1 = ChangeThem_png("关闭按钮1.png");
        surfaceclosebut2 = ChangeThem_png("关闭按钮2.png");
        surfaceclosebut3 = ChangeThem_png("关闭按钮3.png");
        surface_calendar = ChangeThem_png("日历.png");
    }
}


void DecodingRecordText(gchar *text, FriendInfo *info, int count)
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
                    rgba.alpha = 1;
                    rgba.red = colorred / 65535.0;
                    rgba.green = colorgreen / 65535.0;
                    rgba.blue = colorblue / 65535.0;
                    g_object_set(wordtag, "foreground-rgba", &rgba, NULL);
                    ptext = ptext + 6;
                    break;
                }
                case 0:
                {
                    GtkTextChildAnchor *anchor;
                    GtkWidget *image;
                    char filename[256] = {0};
                    unsigned char strdest[16] = {0};
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

}

gboolean show_record_message(void *data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gtk_text_buffer_set_text(record_message->info->record_buffer, "", 0);//发送消息后本地的文本框清0
    for (int i = 0; i < record_message->i; i++)
    {
        if (i == record_message->i - 1)
        {
            record_message->max_id = record_message->record_message_data[i].record_id;
        }
        char nicheng_times[60] = {0};
        char date[50];
        struct tm *p;
        GtkTextIter start, end;
        GtkTextBuffer *buffer;
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_message->info->record_text));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        p = localtime(&record_message->record_message_data[i].time);

        sprintf(date, "  %d/%d/%d \n", p->tm_year + 1900, 1 + p->tm_mon, p->tm_mday);
        gtk_label_set_text(GTK_LABEL(record_message->info->record_date), date);
        PangoFontDescription *font;
        font = pango_font_description_from_string("Mono");//"Mono"字体名
        pango_font_description_set_size(font, 12 * PANGO_SCALE);//设置字体大小
        gtk_widget_override_font(record_message->info->record_date, font);
        gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), record_message->info->record_date, 30, 60);

        if (record_message->record_message_data[i].record_user_uid == record_message->info->user.uid)
        {
            sprintf(nicheng_times,
                    " %s  %d: %d: %d \n", record_message->info->user.nickName,
                    p->tm_hour,
                    p->tm_min,
                    p->tm_sec);
            gtk_text_buffer_insert_with_tags_by_name(buffer, &end,
                                                     nicheng_times, -1, "blue_foreground", "size1", NULL);
        }
        else if (record_message->record_message_data[i].record_user_uid == CurrentUserInfo->uid)
        {
            sprintf(nicheng_times, " %s  %d: %d: %d \n", CurrentUserInfo->nickName, p->tm_hour, p->tm_min, p->tm_sec);
            gtk_text_buffer_insert_with_tags_by_name(buffer, &end,
                                                     nicheng_times, -1, "red_foreground", "size1", NULL);
        }
        DecodingRecordText(record_message->record_message_data[i].record_message_char,
                           record_message->info,
                           record_message->record_message_data[i].messageLen);
    }
    return 0;
}


//图片处理函数
gboolean deal_with_record_message(void *data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    record_message->imagecount--;
    if (record_message->imagecount == 0 && record_message->record_end_flag == 1)
    {
        g_idle_add(show_record_message, record_message);
    }
    return FALSE;
}

int handle_record_message(CRPBaseHeader *header, void *data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    int ret = 1;
    if (header->packetID == CRP_PACKET_FAILURE)
    {
        CRPPacketFailure *infodata = CRPFailureCast(header);
        log_info("FAILURE reason", infodata->reason);
    }
    log_info("Message", "Packet id :%d,SessionID:%d\n", header->packetID, header->sessionID);

    if (header->packetID == CRP_PACKET_OK)
    {
        record_message->record_end_flag = 1;
        if (record_message->imagecount == 0 && record_message->i != 0)
        {
            g_idle_add(show_record_message, record_message);
        }

    }
    if (header->packetID == CRP_PACKET_MESSAGE_RECORD_DATA)
    {

        CRPPacketMessageRecordData *packet = CRPMessageRecordDataCast(header);
        if ((void *) packet != header->data)
        {
            free(packet);
        }
        else
        {
            int i = 0;
            while (i < packet->messages.messageLen)
            {
                if (packet->messages.content[i] != '\0')
                {
                    i++;
                }
                else
                {

                    switch (packet->messages.content[i + 1])
                    {
                        case 1:
                        {
                            i++;
                            while (packet->messages.content[i] != '\0')
                            {
                                i++;
                            }
                            i++;
                            break;
                        };
                        case 2 :
                        {
                            i += 3;
                        };
                        case 3:   //宽度
                        {
                            i += 4;
                            break;
                        };
                        case 4: //字体大小
                        {
                            i += 3;
                            break;
                        };
                        case 5: //颜色
                        {
                            i += 8;

                            break;
                        };
                        case 0 :
                        {
                            char strdest[17] = {0};
                            i += 2;
                            record_message->imagecount++;
                            memcpy(strdest, packet->messages.content + i, 16);
                            FindImage(strdest, record_message, deal_with_record_message); //请求图片
                            i = i + 16;
                            break;
                        }
                        default:
                        {
                            break;
                        };
                    }
                }

            }
            if (record_message->i == 0)
            {
                record_message->min_id = packet->messages.id;
            }
            record_message->record_message_data[record_message->i].record_id = packet->messages.id;
            record_message->record_message_data[record_message->i].time = packet->messages.time;
            record_message->record_message_data[record_message->i].messageLen = packet->messages.messageLen;
            record_message->record_message_data[record_message->i].messageType = packet->messages.messageType;
            record_message->record_message_data[record_message->i].record_user_uid = packet->messages.from;
            record_message->record_message_data[record_message->i].record_message_char = (gchar *) malloc(packet->messages.messageLen);
            memcpy(record_message->record_message_data[record_message->i].record_message_char,
                   packet->messages.content, packet->messages.messageLen);
            record_message->i++;
        }
    }

    if (record_message->info->record_window == NULL)
    {
        ret = 0;
    }
    return ret;
}

void get_record(RcordMessage *record_message, MessageQueryCondition *message_query_conditon)
{

    record_message->i = 0;
    record_message->record_end_flag = 0;
    record_message->imagecount = 0;
    CRPMessageRecordQuerySend(sockfd, record_message->info->session_id, message_query_conditon);
    free(message_query_conditon);
}


//背景的eventbox
static gint record_background_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        if (record_message->info->calendar != NULL)
        {
            gtk_widget_destroy(record_message->info->calendar);
            record_message->info->calendar = NULL;
        }

        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;

}

//下翻页
//鼠标点击事件
static gint record_next_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                              gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }

    return 0;
}

//下翻页
//鼠标抬起事件
static gint record_next_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        MessageQueryCondition *message_query_conditon = (MessageQueryCondition *) malloc(sizeof(MessageQueryCondition));
        message_query_conditon->from = record_message->info->user.uid;
        message_query_conditon->to = record_message->info->user.uid;
        message_query_conditon->time = -1;
        message_query_conditon->timeOperator = -2;
        message_query_conditon->limit = 10;
        message_query_conditon->messageType = UMT_TEXT;
        message_query_conditon->id = record_message->max_id;
        message_query_conditon->idOperator = 2;
        message_query_conditon->fromtoOperator = 4;
        get_record(record_message, message_query_conditon);

    }
    return 0;

}

//下翻页
//鼠标移动事件
static gint record_next_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                          gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//下翻页
//设置离开组件事件
static gint record_next_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window), gdk_cursor_new(GDK_ARROW));
    return 0;
}


//上翻页
//鼠标点击事件
static gint upward_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                              gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }

    return 0;
}

//上翻页
//鼠标抬起事件
static gint upward_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标
    {

        MessageQueryCondition *message_query_conditon = (MessageQueryCondition *) malloc(sizeof(MessageQueryCondition));
        message_query_conditon->from = record_message->info->user.uid;
        message_query_conditon->to = record_message->info->user.uid;
        message_query_conditon->time = -1;
        message_query_conditon->timeOperator = -2;
        message_query_conditon->limit = 10;
        message_query_conditon->messageType = UMT_TEXT;
        message_query_conditon->id = record_message->min_id;
        message_query_conditon->idOperator = -2;
        message_query_conditon->fromtoOperator = 4;
        get_record(record_message, message_query_conditon);

    }
    return 0;

}

//上翻页
//鼠标移动事件
static gint upward_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;

    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                          gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//上翻页
//设置离开组件事件
static gint upward_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

//选择指定日期时的聊天记录
static void calendar_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    struct tm ptime;
    time_t times;
    guint year;
    guint month;
    guint day;
    gtk_calendar_get_date(GTK_CALENDAR(record_message->info->calendar), &year, &month, &day);/*取得选择的年月日*/
    ptime.tm_year = year;
    ptime.tm_mon = month;
    ptime.tm_mday = day;
    times = mktime(&ptime);
    gtk_widget_destroy(widget);
    record_message->info->calendar = NULL;
    MessageQueryCondition *message_query_conditon = (MessageQueryCondition *) malloc(sizeof(MessageQueryCondition));
    message_query_conditon->from = record_message->info->user.uid;
    message_query_conditon->to = record_message->info->user.uid;
    message_query_conditon->time = times;
    message_query_conditon->timeOperator = 1;
    message_query_conditon->limit = 10;
    message_query_conditon->messageType = UMT_TEXT;
    message_query_conditon->id = -1;
    message_query_conditon->idOperator = -2;
    message_query_conditon->fromtoOperator = 4;
    get_record(record_message, message_query_conditon);


}

//日历
//鼠标点击事件
static gint calendar_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;

    if (event->button == 1)
    {     //设置发送按钮
        gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                              gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//鼠标抬起事件
static gint calendar_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        record_message->info->calendar = gtk_calendar_new();
        gtk_widget_show(record_message->info->calendar);
        gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), record_message->info->calendar, 20, 60);
        g_signal_connect(record_message->info->calendar,
                         "day-selected-double-click",
                         G_CALLBACK(calendar_event),
                         record_message);
    }
    return 0;

}

//
//鼠标移动事件
static gint calendar_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                          gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    return 0;
}

//设置离开组件事件
static gint calendar_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window), gdk_cursor_new(GDK_ARROW));
    return 0;
}

//右上关闭
//鼠标点击事件
static gint close_but_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    if (event->button == 1)
    {         //设置右上关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                              gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) record_message->info->image_record_close, surfaceclosebut2); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint close_but_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;

    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                              gdk_cursor_new(GDK_ARROW));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) record_message->info->image_record_close, surfaceclosebut1);  //设置右上关闭按钮
        gtk_widget_destroy(record_message->info->record_window);
        record_message->info->record_window = NULL;
    }
    return 0;

}

//鼠标移动事件
static gint close_but_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window),
                          gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    gtk_image_set_from_surface((GtkImage *) record_message->info->image_record_close, surfaceclosebut3); //置换图标
    return 0;
}

//鼠标likai事件
static gint close_but_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    RcordMessage *record_message = (RcordMessage *) data;
    gdk_window_set_cursor(gtk_widget_get_window(record_message->info->record_window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) record_message->info->image_record_close, surfaceclosebut1);  //设置右上关闭按钮
    return 0;
}

static gint text_view_click(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    FriendInfo *info = (FriendInfo *) data;
    if (info->calendar != NULL)
    {
        gtk_widget_destroy(info->calendar);
        info->calendar = NULL;
    }
    return 0;
}


void ChartRecord(FriendInfo *info)
{
    GtkEventBox *record_background_event_box, *record_next_event_box, *record_close_event_box, *calendar_event_box, *record_upward_event_box;
    GtkWidget *image_calendar, *image_record_background, *image_record_upward;

    RcordMessage *record_message = (struct RcordMessage *) malloc(sizeof(struct RcordMessage));
    record_message->info = info;
    record_message->info->session_id = CountSessionId();

    //创建窗口，并为窗口的关闭信号加回调函数以便退出
    record_message->info->record_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    record_message->info->record_layout = gtk_layout_new(NULL, NULL);
    record_message->info->record_layout2 = gtk_fixed_new();

    gtk_container_add(GTK_CONTAINER (record_message->info->record_window),
                      record_message->info->record_layout);//chartlayout 加入到window
    gtk_container_add(GTK_CONTAINER (record_message->info->record_layout), record_message->info->record_layout2);

    g_signal_connect(G_OBJECT(record_message->info->record_window), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_default_size(GTK_WINDOW(record_message->info->record_window), 318, 550);
    gtk_window_set_position(GTK_WINDOW(record_message->info->record_window), GTK_WIN_POS_MOUSE);//窗口出现位置


    gtk_window_set_decorated(GTK_WINDOW(record_message->info->record_window), FALSE);   // 去掉边框
    create_surfaces();
    image_record_background = gtk_image_new_from_surface(surface_record_background);
    record_message->info->image_record_next = gtk_image_new_from_surface(surface_next);
    record_message->info->image_record_close = gtk_image_new_from_surface(surfaceclosebut1);
    image_calendar = gtk_image_new_from_surface(surface_calendar);
    image_record_upward = gtk_image_new_from_surface(surface_upward);
// 设置窗体获取鼠标事件 背景

    record_background_event_box = BuildEventBox(
            image_record_background,
            G_CALLBACK(record_background_button_press_event),
            NULL, NULL, NULL, NULL, record_message);

    //翻页
    record_next_event_box = BuildEventBox(
            record_message->info->image_record_next,
            G_CALLBACK(record_next_button_press_event),
            G_CALLBACK(record_next_enter_notify_event),
            G_CALLBACK(record_next_leave_notify_event),
            G_CALLBACK(record_next_button_release_event),
            NULL,
            record_message);
    calendar_event_box = BuildEventBox(
            image_calendar,
            G_CALLBACK(calendar_button_press_event),
            G_CALLBACK(calendar_enter_notify_event),
            G_CALLBACK(calendar_leave_notify_event),
            G_CALLBACK(calendar_button_release_event),
            NULL,
            record_message);
    record_close_event_box = BuildEventBox(
            record_message->info->image_record_close,
            G_CALLBACK(close_but_button_press_event),
            G_CALLBACK(close_but_enter_notify_event),
            G_CALLBACK(close_but_leave_notify_event),
            G_CALLBACK(close_but_button_release_event),
            NULL,
            record_message);

    record_upward_event_box = BuildEventBox(
            image_record_upward,
            G_CALLBACK(upward_button_press_event),
            G_CALLBACK(upward_enter_notify_event),
            G_CALLBACK(upward_leave_notify_event),
            G_CALLBACK(upward_button_release_event),
            NULL,
            record_message);
    //背景
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), GTK_WIDGET(record_background_event_box), 0, 0);
    //聊天记录下翻页
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), GTK_WIDGET(record_next_event_box), 280, 510);
    //上翻页
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), GTK_WIDGET(record_upward_event_box), 10, 510);
    //右上角关闭按钮
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), GTK_WIDGET(record_close_event_box), 280, 0);
    //日历
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), GTK_WIDGET(calendar_event_box), 2, 60);

    //创建文本框

    record_message->info->record_text = gtk_text_view_new();
    record_message->info->record_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (record_message->info->record_text));
    g_signal_connect(record_message->info->record_text,
                     "button_press_event",
                     G_CALLBACK(text_view_click),
                     record_message->info);

    //创建文字标记
    gtk_text_buffer_create_tag(record_message->info->record_buffer, "red_foreground", "foreground", "red", NULL);
    gtk_text_buffer_create_tag(record_message->info->record_buffer, "gray_foreground", "foreground", "gray", NULL);
    gtk_text_buffer_create_tag(record_message->info->record_buffer, "blue_foreground", "foreground", "blue", NULL);
    gtk_text_buffer_create_tag(record_message->info->record_buffer, "size1", "font", "12", NULL);
    //创建滚动窗口
    record_message->info->record_sw = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_container_add(GTK_CONTAINER(record_message->info->record_sw), record_message->info->record_text);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(record_message->info->record_text), GTK_WRAP_WORD_CHAR);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(record_message->info->record_text), 0);//不可编辑
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(record_message->info->record_text), FALSE);

    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2),
                  GTK_WIDGET(record_message->info->record_sw),
                  2,
                  100);//文本框位置

    gtk_widget_set_size_request(GTK_WIDGET(record_message->info->record_sw), 315, 380);
    GdkRGBA rgba = {0.92, 0.88, 0.74, 1};
    gtk_widget_override_background_color(record_message->info->record_text, GTK_STATE_FLAG_NORMAL, &rgba);//设置透明

//获取当前时间
    char date[40] = {0};
    time_t date_time;
    time(&date_time);
    struct tm *p;
    p = localtime(&date_time);
    sprintf(date, "  %d / %d / %d \n", (1900 + p->tm_year), 1 + p->tm_mon, p->tm_mday);
    record_message->info->record_date = gtk_label_new(date);
    PangoFontDescription *font;
    font = pango_font_description_from_string("Mono");//"Mono"字体名
    pango_font_description_set_size(font, 12 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(record_message->info->record_date, font);
    gtk_fixed_put(GTK_FIXED(record_message->info->record_layout2), record_message->info->record_date, 30, 60);

    AddMessageNode(record_message->info->session_id, handle_record_message, record_message);

    //当刚打开聊天记录时初始化

    MessageQueryCondition *message_query_conditon = (MessageQueryCondition *) malloc(sizeof(MessageQueryCondition));
    message_query_conditon->from = record_message->info->user.uid;
    message_query_conditon->to = record_message->info->user.uid;
    message_query_conditon->time = date_time;
    message_query_conditon->timeOperator = -2;
    message_query_conditon->limit = 10;
    message_query_conditon->messageType = UMT_TEXT;
    message_query_conditon->id = -1;
    message_query_conditon->idOperator = -2;
    message_query_conditon->fromtoOperator = 4;
    get_record(record_message, message_query_conditon);
    gtk_widget_show_all(record_message->info->record_window);
}