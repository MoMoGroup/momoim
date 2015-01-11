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
#include "PopupWinds.h"

GtkWidget *addwindow, *addframelayout;
GtkWidget *addlayout2, *addlayout1, *addlayout31;//layout

GtkWidget *addtext;

cairo_surface_t *surfacebiaoji, *surfacenext, *surfacenext_press, *surfaceclose;
cairo_surface_t *surfacebackground1, *surfacebackground3, *surfacebackground2, *surfacedone, *surfacedone2;
cairo_surface_t *surfacehead;
//资源

GtkWidget *background1, *background2, *background3, *biaoji1, *biaoji2, *next, *addclose;    //引用
GtkWidget *smallhead, *done, *done2;

GtkEventBox *next_enent_box, *close_event_box;
GtkEventBox *next_enent_box2, *close_event_box2, *add_mov_event;
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
    surfacebackground1 = cairo_image_surface_create_from_png("查找背景1.png");
    surfacebackground2 = cairo_image_surface_create_from_png("查找背景2.png");
    surfacebackground3 = cairo_image_surface_create_from_png("查找背景3.png");

    surfacebiaoji = cairo_image_surface_create_from_png("标记.png");
    surfacenext = cairo_image_surface_create_from_png("下一步.png");
    surfacenext_press = cairo_image_surface_create_from_png("下一步2.png");
    surfaceclose = cairo_image_surface_create_from_png("关闭按钮1.png");

    surfacedone = cairo_image_surface_create_from_png("完成.png");
    surfacedone2 = cairo_image_surface_create_from_png("完成2.png");

    //获得
    background1 = gtk_image_new_from_surface(surfacebackground1);
    background2 = gtk_image_new_from_surface(surfacebackground2);

    biaoji1 = gtk_image_new_from_surface(surfacebiaoji);
    biaoji2 = gtk_image_new_from_surface(surfacebiaoji);

    next = gtk_image_new_from_surface(surfacenext);
    addclose = gtk_image_new_from_surface(surfaceclose);
    done = gtk_image_new_from_surface(surfacedone);
    done2 = gtk_image_new_from_surface(surfacedone2);

}

//拖拽
static gint add_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}

//关闭按钮
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    AddFriendflag = 1;//判断是否打开搜索窗口,置1，可以打开了
    gtk_widget_destroy(addwindow);
    return 0;
}


//完成
static gint done_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    struct add_friend_info *p = data;
    if (p->uid == CurrentUserInfo->uid) {
        popup("", "不能添加自己为好友");
    }
    else {
        CRPFriendAddSend(sockfd, p->sessionid, p->uid, p->note);//发送添加请求
        AddFriendflag = 1;//判断是否打开搜索窗口,置1，可以打开了
        gtk_widget_destroy(addwindow);
    }
}
//
//static gint done2_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
//{
////    struct add_friend_info *p = data;
////    CRPFriendAddSend(sockfd, p->sessionid, p->uid, p->note);//发送添加请求
////    gtk_widget_destroy(addwindow);
//}

static gint next2_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    struct add_friend_info *p = data;
    GtkEntryBuffer *buf = gtk_entry_get_buffer(yanzhengxinxi);
    p->note = gtk_entry_buffer_get_text(buf);
//    create_surface();
//    gtk_fixed_put(GTK_FIXED(addlayout2), background3, 0, 0);
//
    done_event_box = BuildEventBox(done,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(done_button_release_event),
            NULL,
            p);

//
//    gtk_fixed_put(GTK_FIXED(addlayout2), done_event_box, 400, 200);
//  //  gtk_container_add(GTK_CONTAINER(addframelayout), addlayout3);
//    gtk_widget_show_all(addlayout2);
    //gtk_widget_destroy(addlayout2);
    //gtk_widget_hide(addlayout2);

    background3 = gtk_image_new_from_surface(surfacebackground3);
    addclose = gtk_image_new_from_surface(surfaceclose);

    gtk_fixed_put(GTK_FIXED(addlayout2), background3, 183, 0);
    gtk_fixed_put(GTK_FIXED(addlayout2), addclose, 519, 0);
    gtk_fixed_put(GTK_FIXED(addlayout2), done_event_box, 400, 200);
    //gtk_container_add(GTK_CONTAINER (addframelayout), addlayout2);
    gtk_widget_show_all(addframelayout);

    return 0;
}

//第2步put图片
gboolean first(gpointer user_data)
{

    return FALSE;
}


gboolean putimage(gpointer user_data)
{
    addlayout2 = gtk_fixed_new();

    // 设置窗体获取鼠标事件
    add_mov_event = BuildEventBox(
            background2,
            G_CALLBACK(add_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    //gtk_fixed_put(GTK_FIXED(popuplayout), pop_mov_event, 0, 0);
    //  gtk_fixed_put(GTK_FIXED(addlayout1), add_mov_event, 0, 0);

    gtk_fixed_put(GTK_FIXED(addlayout2), add_mov_event, 0, 0);

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
    //gtk_entry_set_wrap_mode(yanzhengxinxi, GTK_WRAP_WORD_CHAR);//自动换行

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
            NULL,
            NULL);
    next_enent_box2 = BuildEventBox(
            next,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(next2_button_release_event),
            NULL,
            p);

    gtk_fixed_put(GTK_FIXED(addlayout2), (GtkWidget *) next_enent_box2, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout2), (GtkWidget *) close_event_box2, 519, 0);

    //gtk_widget_hide(addlayout1);//隐藏1
    gtk_widget_destroy(addlayout1);
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout2);

    gtk_widget_show_all(addlayout2);//显示2
    return FALSE;
}


static int searchfriend(CRPBaseHeader *header, void *data)//接收查找好友的资料
{
    struct add_friend_info *p = (struct add_friend_info *) data;
    switch (header->packetID)
    {
        case CRP_PACKET_OK:
        {
            log_info("更新成功", "OKBAO\n");
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

            //显示图片
            FindImage(p->key, data, putimage);
            break;


        };
        default:
        {

            break;

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

//构造第一个界面的地方
int AddFriendFun()
{
    AddFriendflag = 0;//判断是否打开搜索窗口，置0，不能打开
    addwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    addframelayout = gtk_layout_new(NULL, NULL);
    addlayout1 = gtk_fixed_new();


    gtk_window_set_position(GTK_WINDOW(addwindow), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (addwindow), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(addwindow), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(addwindow), 560, 270);//窗口大小


    create_surface();
    //第一个界面de 下一步
    next_enent_box = BuildEventBox(
            next,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(next_button_release_event),
            NULL,
            NULL
    );
    //第一个界面的关闭
    close_event_box = BuildEventBox(
            addclose,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(close_button_release_event),
            NULL,
            NULL
    );

    // 设置窗体获取鼠标事件
    add_mov_event = BuildEventBox(
            background1,
            G_CALLBACK(add_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    //gtk_fixed_put(GTK_FIXED(popuplayout), pop_mov_event, 0, 0);
    gtk_fixed_put(GTK_FIXED(addlayout1), add_mov_event, 0, 0);

    gtk_fixed_put(GTK_FIXED(addlayout1), biaoji1, 6, 75);
    gtk_fixed_put(GTK_FIXED(addlayout1), biaoji2, 6, 118);

    gtk_fixed_put(GTK_FIXED(addlayout1), next_enent_box, 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout1), close_event_box, 519, 0);


    addtext = gtk_entry_new();//帐号输入
    gtk_fixed_put(GTK_FIXED(addlayout1), addtext, 170, 80);
    gtk_test_text_set(addtext, "10000");

    gtk_container_add(GTK_CONTAINER (addwindow), addframelayout);
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout1);

    gtk_widget_show_all(addwindow);

}

//以下函数为添加好友提示框，同意或者或略。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。


GtkEventBox *popup_accept_eventbox, *popup_cancel_eventbox, *pop_mov_event;
GtkWidget *popupwindow, *popupframelayout, *popuplayout;
cairo_surface_t *popupsurfacecancel, *popupsurfacedone;
cairo_surface_t *popupsurfacebackground;

//取消的话直接销毁
gint popup_cancel(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gtk_widget_destroy(popupwindow);
    return 0;
}

gint popup_done(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
//    char *packet = malloc(sizeof(CRPPacketMessageNormal));
//    CRPFriendAcceptSend(sockfd, 1, packet->uid);//同意的话发送Accept
//    cairo_surface_t *popupsurfacedone2;
//    GtkWidget *popupdone2;
//    popupsurfacedone2 = cairo_image_surface_create_from_png("同意2.png");
//    popupdone2 = gtk_image_new_from_surface(popupsurfacedone2);
//    gtk_fixed_put(GTK_FIXED(popuplayout), popupdone2, 150, 170);

    uint32_t uid = data;
    CRPFriendAcceptSend(sockfd, 1, uid);//同意的话发送Accept
    gtk_widget_destroy(popupwindow);
    return 0;
}

//背景的eventbox拖曳窗口
static gint pop_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(popupwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;
}


int Friend_Fequest_Popup(uint32_t uid, const char *verification_message)
{

    GtkWidget *popupcancel, *popupdone, *popupbackground;


    popupwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    popupframelayout = gtk_layout_new(NULL, NULL);
    popuplayout = gtk_fixed_new();

    gtk_window_set_position(GTK_WINDOW(popupwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (popupwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(popupwindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(popupwindow), 250, 235);


    popupsurfacecancel = cairo_image_surface_create_from_png("忽略1.png");
    popupsurfacedone = cairo_image_surface_create_from_png("同意1.png");

    popupsurfacebackground = cairo_image_surface_create_from_png("提示框.png");
    //获得
    popupcancel = gtk_image_new_from_surface(popupsurfacecancel);
    popupdone = gtk_image_new_from_surface(popupsurfacedone);


    popupbackground = gtk_image_new_from_surface(popupsurfacebackground);



    // 设置窗体获取鼠标事件
    pop_mov_event = BuildEventBox(
            popupbackground,
            G_CALLBACK(pop_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    gtk_fixed_put(GTK_FIXED(popuplayout), pop_mov_event, 0, 0);

    // gtk_fixed_put(GTK_FIXED(popuplayout), popupbackground, 0, 0);



    popup_cancel_eventbox = BuildEventBox(
            popupcancel,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(popup_cancel),
            NULL,
            NULL
    );
    popup_accept_eventbox = BuildEventBox(
            popupdone,
            NULL,
            NULL,
            NULL,
            G_CALLBACK(popup_done),
            NULL,
            uid
    );

    GtkTextView *text, *yanzheng;
    // GtkWidget *text,*yanzheng;
    text = gtk_text_view_new();
    yanzheng = gtk_text_view_new();
    char mes[80];
    sprintf(mes, "用户%d请求添加你为好友", uid);
    gtk_test_text_set(text, mes);
//    sprintf(mes, "系统消息");
    gtk_test_text_set(yanzheng, verification_message);

    GdkRGBA rgba = {0.92, 0.88, 0.74, 1};
    gtk_widget_override_background_color(text, GTK_STATE_NORMAL, &rgba);//设置透明
    //gtk_widget_override_background_color(title, GTK_STATE_NORMAL, &rgba);//设置透明

    gtk_fixed_put(GTK_FIXED(popuplayout), text, 30, 70);
    gtk_fixed_put(GTK_FIXED(popuplayout), yanzheng, 40, 120);


    gtk_fixed_put(GTK_FIXED(popuplayout), popup_cancel_eventbox, 30, 170);
    gtk_fixed_put(GTK_FIXED(popuplayout), popup_accept_eventbox, 150, 170);


    gtk_container_add(GTK_CONTAINER (popupwindow), popupframelayout);
    gtk_container_add(GTK_CONTAINER (popupframelayout), popuplayout);

    gtk_widget_show_all(popupwindow);


    return 0;
}
