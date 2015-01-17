#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <protocol/info/Request.h>
#include <protocol/CRPPackets.h>
#include <glib-unix.h>
#include <math.h>
#include <cairo-script-interpreter.h>
#include "friend.h"
#include "common.h"
#include "../managegroup/ManageGroup.h"

static GtkWidget *addwindow, *addframelayout;
static GtkWidget *addlayout2, *addlayout1, *addlayout31;
//layout
static GtkWidget *addtext;
static cairo_surface_t *surfacenext, *surfacenext_press, *surfaceclose;
static cairo_surface_t *surfacebackground1, *surfacebackground3, *surfacebackground2, *surfacedone, *surfacedone2;
static cairo_surface_t *surfacehead;
//资源
static GtkWidget *background1, *background2, *background3, *next, *addclose;    //引用
static GtkWidget *smallhead, *done, *done2;
static GtkEventBox *next_enent_box, *close_event_box;
static GtkEventBox *next_enent_box2, *close_event_box2, *add_mov_event;
static GtkEventBox *done_event_box;
static GtkWidget *yanzhengxinxi;
//事件盒子



typedef struct add_friend_info
{
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
//查找背景
    surfacebackground1 = ChangeThem_png("查找背景1.png");
    surfacebackground2 = ChangeThem_png("查找背景2.png");
    surfacebackground3 = ChangeThem_png("查找背景3.png");
    surfacenext = ChangeThem_png("下一步.png");
    surfacenext_press = ChangeThem_png("下一步2.png");
    surfaceclose = ChangeThem_png("关闭按钮1.png");
    surfacedone = ChangeThem_png("完成.png");
    surfacedone2 = ChangeThem_png("完成2.png");

    //获得
    background1 = gtk_image_new_from_surface(surfacebackground1);
    background2 = gtk_image_new_from_surface(surfacebackground2);
    next = gtk_image_new_from_surface(surfacenext);
    addclose = gtk_image_new_from_surface(surfaceclose);
    done = gtk_image_new_from_surface(surfacedone);
    done2 = gtk_image_new_from_surface(surfacedone2);

}

//拖拽主窗口事件
static gint add_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

//关闭按钮3个事件
//关闭按钮放上去
static gint close_button_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_close2 = ChangeThem_png("关闭按钮2.png");
    gtk_image_set_from_surface((GtkImage *) addclose, surface_close2);
    return 0;
}

//关闭按钮移走
static gint close_button_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_close1 = ChangeThem_png("关闭按钮1.png");
    gtk_image_set_from_surface((GtkImage *) addclose, surface_close1);
    return 0;
}

//关闭按钮按下
static gint close_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    AddFriendflag = 1;//判断是否打开搜索窗口,置1，可以打开了
    gtk_widget_destroy(addwindow);
    return 0;
}

//完成添加按钮3个事件
//完成放上去
static gint done_button_notify_event(GtkWidget *widget, GdkEventButton *event,
                                     gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_done2 = ChangeThem_png("完成2.png");
    gtk_image_set_from_surface((GtkImage *) done, surface_done2);
    return 0;
}

//完成移走
static gint done_button_leave_event(GtkWidget *widget, GdkEventButton *event,
                                    gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_done1 = ChangeThem_png("完成.png");
    gtk_image_set_from_surface((GtkImage *) done, surface_done1);
    return 0;
}

//完成按下
static gint done_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    struct add_friend_info *p = data;

    CRPFriendAddSend(sockfd, p->sessionid, p->uid, p->note);//发送添加请求
    AddFriendflag = 1;//判断是否打开搜索窗口,置1，可以打开了
    gtk_widget_destroy(addwindow);

}

//通用下一步的2个事件
//下一步放上去
static gint next_button_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_next2 = ChangeThem_png("下一步2.png");
    gtk_image_set_from_surface((GtkImage *) next, surface_next2);
    return 0;
}

//下一步移走
static gint next_button_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(addwindow), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_next1 = ChangeThem_png("下一步.png");
    gtk_image_set_from_surface((GtkImage *) next, surface_next1);
    return 0;
}

//第二个下一步的事件
static gint next2_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    struct add_friend_info *p;
    p = data;
    if (p->uid == CurrentUserInfo->uid)
    {
        g_idle_add(GroupPop, "不能添加自己为好友");
    }
    else
    {
        GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(yanzhengxinxi));
        p->note = (char *) gtk_entry_buffer_get_text(buf);


        done_event_box = BuildEventBox(done,
                                       NULL,
                                       G_CALLBACK(done_button_notify_event),
                                       G_CALLBACK(done_button_leave_event),
                                       G_CALLBACK(done_button_release_event),
                                       NULL,
                                       p);


        background3 = gtk_image_new_from_surface(surfacebackground3);
        addclose = gtk_image_new_from_surface(surfaceclose);

        gtk_fixed_put(GTK_FIXED(addlayout2), background3, 183, 0);
        gtk_fixed_put(GTK_FIXED(addlayout2), addclose, 519, 0);
        gtk_fixed_put(GTK_FIXED(addlayout2), GTK_WIDGET(done_event_box), 400, 200);
        gtk_widget_show_all(addframelayout);
    }
    return 0;
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

    gtk_fixed_put(GTK_FIXED(addlayout2), GTK_WIDGET(add_mov_event), 0, 0);

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

//第2个界面的关闭，下一步事件
    close_event_box2 = BuildEventBox(
            addclose,
            NULL,
            G_CALLBACK(close_button_notify_event),
            G_CALLBACK(close_button_leave_event),
            G_CALLBACK(close_button_release_event),
            NULL,
            NULL);

    next_enent_box2 = BuildEventBox(
            next,
            NULL,
            G_CALLBACK(next_button_notify_event),
            G_CALLBACK(next_button_leave_event),
            G_CALLBACK(next2_button_release_event),
            NULL,
            p);


//    cairo_surface_t *surfaceback;
//    GtkWidget *back;
//    surfaceback= ChangeThem_png("上一步.png");
//    back= gtk_image_new_from_surface(surfaceback);


    //gtk_fixed_put(GTK_FIXED(addlayout2), back, 230, 200);
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
            if ((void *) infodata != header->data)
            {
                free(data);
            }
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


//点击下一步开始查找好友资料，主线程，第一个下一步的事件
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
            G_CALLBACK(next_button_notify_event),
            G_CALLBACK(next_button_leave_event),
            G_CALLBACK(next_button_release_event),
            NULL,
            NULL
                                  );
    //第一个界面的关闭
    close_event_box = BuildEventBox(
            addclose,
            NULL,
            G_CALLBACK(close_button_notify_event),
            G_CALLBACK(close_button_leave_event),
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
    gtk_fixed_put(GTK_FIXED(addlayout1), GTK_WIDGET(add_mov_event), 0, 0);

    gtk_fixed_put(GTK_FIXED(addlayout1), GTK_WIDGET(next_enent_box), 400, 200);
    gtk_fixed_put(GTK_FIXED(addlayout1), GTK_WIDGET(close_event_box), 519, 0);


    addtext = gtk_entry_new();//帐号输入
    gtk_fixed_put(GTK_FIXED(addlayout1), addtext, 170, 80);
    gtk_test_text_set(addtext, "10000");

    gtk_container_add(GTK_CONTAINER (addwindow), addframelayout);
    gtk_container_add(GTK_CONTAINER (addframelayout), addlayout1);

    gtk_widget_show_all(addwindow);

}

