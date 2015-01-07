#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <protocol/info/Request.h>
#include <protocol/CRPPackets.h>
#include <glib-unix.h>
#include <math.h>
#include <cairo-script-interpreter.h>
#include "addfriend.h"
#include "common.h"
#include "ClientSockfd.h"
#include "MainInterface.h"

int searchfriend(CRPBaseHeader *header, void *data);

GtkWidget *addwindow, *addframelayout;
GtkWidget *addlayout2, *addlayout1, *addlayout3;//layout

GtkWidget *addtext;

cairo_surface_t *surfacebiaoji, *surfacenext, *surfacenext_press, *surfaceclose;
cairo_surface_t *surfacebackground, *surfacebackground0, *surfacebackground3, *surfacedone,*surfacedone2;
cairo_surface_t *surfacehead;
//资源

GtkWidget *background, *background3, *biaoji, *next, *next_press, *addclose;    //引用
GtkWidget  *smallhead, *done,*done2;

GtkEventBox *next_enent_box, *close_event_box;
GtkEventBox *next_enent_box2, *close_event_box2;
GtkEventBox *done_event_box;

GtkWidget *yanzhengxinxi;
//事件盒子

GtkWidget *background0;

typedef struct add_friend_info {
    FILE *fp;
    char *nickname;
    uint32_t uid;
    char key[16];
    session_id_t sessionid;
    char *note;
};

void create_surface()
{
    //加载资源，第1个
    surfacebackground = cairo_image_surface_create_from_png("查找背景2.png");
    surfacebackground3 = cairo_image_surface_create_from_png("查找.png");
    surfacebiaoji = cairo_image_surface_create_from_png("标记.png");
    surfacenext = cairo_image_surface_create_from_png("下一步.png");
    surfacenext_press = cairo_image_surface_create_from_png("下一步2.png");
    surfaceclose = cairo_image_surface_create_from_png("关闭按钮1.png");
    surfacedone = cairo_image_surface_create_from_png("完成.png");
    surfacedone2=cairo_image_surface_create_from_png("完成2.png");

    //获得
    background = gtk_image_new_from_surface(surfacebackground);
    background3 = gtk_image_new_from_surface(surfacebackground3);
    biaoji = gtk_image_new_from_surface(surfacebiaoji);
    next = gtk_image_new_from_surface(surfacenext);
    addclose = gtk_image_new_from_surface(surfaceclose);
    done = gtk_image_new_from_surface(surfacedone);
    done2= gtk_image_new_from_surface(surfacedone2);

}


//关闭按钮
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gtk_widget_destroy(addwindow);
    return 0;
}


//完成
static gint done_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    struct add_friend_info *p = data;
    CRPFriendAddSend(sockfd, p->sessionid, p->uid, p->note);//发送添加请求
    gtk_widget_destroy(addwindow);
}
static gint done2_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
//    struct add_friend_info *p = data;
//    CRPFriendAddSend(sockfd, p->sessionid, p->uid, p->note);//发送添加请求
//    gtk_widget_destroy(addwindow);
}

static gint next2_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    struct add_friend_info *p = data;
    GtkEntryBuffer *buf=gtk_entry_get_buffer(yanzhengxinxi);
    p->note=gtk_entry_buffer_get_text(buf);
    gtk_widget_hide(addlayout2);
    addlayout3 = gtk_fixed_new();
    create_surface();
    gtk_fixed_put(GTK_FIXED(addlayout3), background3, 0, 0);

    done_event_box = BuildEventBox(
            done,
            G_CALLBACK(done2_button_release_event),
            NULL,
            NULL,
            G_CALLBACK(done_button_release_event),
            p
    );
//    closebut_event_box = BuildEventBox(
//            closebut,
//            G_CALLBACK(closebut_button_press_event),
//            G_CALLBACK(closebut_enter_notify_event),
//            G_CALLBACK(closebut_leave_notify_event),
//            G_CALLBACK(closebut_button_release_event),
//            NULL);
    gtk_fixed_put(GTK_FIXED(addlayout3), done_event_box, 400, 200);
    gtk_container_add(GTK_CONTAINER(addframelayout), addlayout3);
    gtk_widget_show_all(addlayout3);

    return 0;
}


gboolean first(gpointer user_data)
{
    addlayout2 = gtk_fixed_new();
    surfacebackground0 = cairo_image_surface_create_from_png("查找2.png");
    background0 = gtk_image_new_from_surface(surfacebackground0);

    gtk_fixed_put(GTK_FIXED(addlayout2), background0, 0, 0);
    return FALSE;
}


gboolean putimage(gpointer user_data)
{
    struct add_friend_info *p = user_data;
    static cairo_t *cr;
    cairo_surface_t *surface;

    //显示ID
    char addidstring[80] = {0};
    GtkWidget *idtext;
    sprintf(addidstring, "%d", p->uid);

    idtext = gtk_label_new(addidstring);
    PangoFontDescription *font1;
    font1 = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font1, 16 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(idtext, font1);

    gtk_fixed_put(GTK_FIXED(addlayout2), idtext, 63, 155);

    //把昵称显示出来
    GtkWidget *nicheng;
    nicheng = gtk_label_new(p->nickname);
    free(p->nickname);
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(nicheng, font);
    gtk_fixed_put(GTK_FIXED(addlayout2), nicheng, 65, 180);


    yanzhengxinxi = gtk_entry_new();
    //yanzhengxinxi=GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_text_view_set_wrap_mode(yanzhengxinxi, GTK_WRAP_WORD_CHAR);//自动换行

    gtk_widget_set_size_request(yanzhengxinxi, 220, 90);//验证信息窗口大小
    gtk_fixed_put(GTK_FIXED(addlayout2), yanzhengxinxi, 210, 75);//位置

    char filename[256];
    HexadecimalConversion(filename, p->key);
    surface = cairo_image_surface_create_from_png(filename);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);

    //创建画布
    surfacehead = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 300, 150);
    //创建画笔
    cr = cairo_create(surfacehead);
    //缩放
    cairo_arc(cr, 60, 60, 60, 0, M_PI * 2);
    cairo_clip(cr);
    cairo_scale(cr, 125.0 / w, 126.0 / h);
    //把画笔和图片相结合。
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    smallhead = gtk_image_new_from_surface(surfacehead);
    gtk_fixed_put(GTK_FIXED(addlayout2), smallhead, 27, 15);
    cairo_destroy(cr);

    create_surface();


    close_event_box2 = BuildEventBox(
            addclose,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(close_button_release_event),
            NULL
    );
    next_enent_box2 = BuildEventBox(
            next,
            NULL, NULL,
            NULL,
            G_CALLBACK(next2_button_release_event),
            p);

    gtk_fixed_put(GTK_FIXED(addlayout2), next_enent_box2, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout2), close_event_box2, 519, 0);

    gtk_widget_hide(addlayout1);//隐藏1
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout2);

    gtk_widget_show_all(addlayout2);//显示2
    return FALSE;
}


int searchfriend(CRPBaseHeader *header, void *data)//接收查找好友的资料
{
    g_idle_add(first, "");
    struct add_friend_info *p = (struct add_friend_info *) data;
    switch (header->packetID)
    {
        case CRP_PACKET_OK:
        {
            log_info("请求添加", "收到OKBAO\n");
            return 0;
        };


        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURe reason", infodata->reason);
            break;
        };
        case CRP_PACKET_INFO_DATA:
        {
            CRPPacketInfoData *infodata = CRPInfoDataCast(header);
            memcpy(p->key, infodata->info.icon, 16);
            CRPFileRequestSend(sockfd, header->sessionID, 0, infodata->info.icon);//发送用户头像请求
            //保存uid
            p->uid = infodata->info.uid;
            p->sessionid = header->sessionID;

            //显示昵称上去
            char *mem = malloc(strlen(infodata->info.nickName) + 1);
            memcpy(mem, infodata->info.nickName, strlen(infodata->info.nickName));
            mem[strlen(infodata->info.nickName)] = 0;
            p->nickname = mem;
            //free(mem);
            if ((const char *) infodata != header->data)
            {
                free(infodata);
            }
            break;


        };
        case CRP_PACKET_FILE_DATA_START:
        {
            CRPPacketFileDataStart *packet = CRPFileDataStartCast(header);
            char filename[256];
            HexadecimalConversion(filename, p->key);
            p->fp = fopen(filename, "w");
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

            fwrite(packet->data, 1, packet->length, p->fp);
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
            fclose(p->fp);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            g_idle_add(putimage, p);
            return 1;
        }

    }
    return 1;
}

//点击下一步开始查找好友资料，主线程
static gint next_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    session_id_t sessionid = CountSessionId();
    //主消息循环注册查找好友
    AddMessageNode(sessionid, searchfriend, malloc(sizeof(struct add_friend_info)));
    //请求要添加的资料
    CRPInfoRequestSend(sockfd, sessionid, atol(gtk_entry_get_text(addtext)));
    return 0;
}

int AddFriendFun()
{
    addwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    addframelayout = gtk_layout_new(NULL, NULL);
    addlayout1 = gtk_fixed_new();


    gtk_window_set_position(GTK_WINDOW(addwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (addwindow), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(addwindow), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(addwindow), 560, 270);//窗口大小


    create_surface();

    next_enent_box = BuildEventBox(
            next,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(next_button_release_event),
            NULL
    );
    close_event_box = BuildEventBox(
            addclose,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(close_button_release_event),
            NULL
    );
    gtk_fixed_put(GTK_FIXED(addlayout1), background, 0, 0);
    gtk_fixed_put(GTK_FIXED(addlayout1), biaoji, 6, 75);
    gtk_fixed_put(GTK_FIXED(addlayout1), next_enent_box, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout1), close_event_box, 519, 0);


    addtext = gtk_entry_new();//帐号输入
    gtk_fixed_put(GTK_FIXED(addlayout1), addtext, 170, 80);
    gtk_test_text_set(addtext, "10000");

    gtk_container_add(GTK_CONTAINER (addwindow), addframelayout);
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout1);

    gtk_widget_show_all(addwindow);


}



