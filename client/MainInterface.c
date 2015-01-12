#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <logger.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "addfriend.h"
#include "chartmessage.h"
#include "onlylookinfo.h"
#include "Managegroup.h"


static GtkWidget *background, *headx, *search, *friend, *closebut;
static GtkWidget *background1, *headx, *search, *friend, *closebut;
static GtkWidget *window;
static GtkTreeView *treeView;
static GtkWidget *frameLayout, *MainLayout;
static cairo_surface_t *surfacemainbackgroud, *surfacehead2, *surfaceresearch, *surfacefriendimage, *surfaceclose51, *surfaceclose52, *surfaceclose53;


GtkTreeStore *TreeViewListStore;
static GdkPixbuf *pixbuf;
static cairo_t *cr;
static GtkWidget *vbox;
static GtkEventBox *closebut_event_box, *background_event_box, *search_event_box, *headx_event_box;


static gint friendListStoreFunc(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
    int64_t priA, priB;
    gtk_tree_model_get(TreeViewListStore, a, PRIORITY_COL, &priA, -1);
    gtk_tree_model_get(TreeViewListStore, b, PRIORITY_COL, &priB, -1);
    return priA - priB;
}

GtkTreeModel *createModel()
{
    int64_t priority;
    gint i, j;
    cairo_surface_t *surface;
    cairo_surface_t *surfaceIcon;

    GtkTreeIter iter1, iter2;

    TreeViewListStore = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_INT64);
    gtk_tree_sortable_set_default_sort_func(TreeViewListStore, friendListStoreFunc, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(TreeViewListStore,
            GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
            GTK_SORT_DESCENDING);

//头节点
    //  groupFriendsInfo_head =(groupFriendsInfo *)calloc(1,sizeof(struct groupFriendsInfo));

    for (i = 0; i < friends->groupCount - 1; i++)
    {
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 33);
        cr = cairo_create(surface);
        cairo_move_to(cr, 0, 20);
        cairo_set_font_size(cr, 14);
        cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_show_text(cr, friends->groups[i].groupName);
        pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 33);
        gtk_tree_store_append(TreeViewListStore, &iter1, NULL);
        gtk_tree_store_set(TreeViewListStore, &iter1,
                PIXBUF_COL, pixbuf,
                FRIENDUID_COL, (uint32_t) friends->groups[i].groupId,
                PRIORITY_COL, (int64_t) -i,
                -1);

        g_object_unref(pixbuf);


        for (j = 0; j < friends->groups[i].friendCount; j++)
        {
            char friendname[20] = {0};
            char mulu[80] = {0};
            sprintf(mulu, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, friends->groups[i].friends[j]);
            pixbuf = gdk_pixbuf_new_from_file(mulu, NULL);

            FriendInfo *rear = FriendInfoHead;

            while (rear)
            {
                if (rear->uid == friends->groups[i].friends[j])
                {
                    memcpy(friendname, rear->user.nickName, sizeof(rear->user.nickName));


                    break;
                }
                rear = rear->next;
            }


            pixbuf = DrawFriend(&rear->user, rear->inonline);

            gtk_tree_store_append(TreeViewListStore, &iter2, &iter1);//
            if (CurrentUserInfo->uid == friends->groups[i].friends[j])//说明是自己
            {

                priority = 5256000;//10年
            }
            else if (rear->inonline)
            {
                priority = 1;
            }
            else
            {
                priority = -1;
            }
            gtk_tree_store_set(TreeViewListStore, &iter2,
                    PIXBUF_COL, pixbuf,
                    FRIENDUID_COL, friends->groups[i].friends[j],
                    PRIORITY_COL, priority,
                            - 1);
            g_object_unref(pixbuf);

        }
    }

    return GTK_TREE_MODEL(TreeViewListStore);
}


static void create_surfaces()
{

    surfacemainbackgroud = cairo_image_surface_create_from_png("主背景.png");

    surfaceresearch = cairo_image_surface_create_from_png("搜索.png");
    surfacefriendimage = cairo_image_surface_create_from_png("好友.png");
    surfaceclose51 = cairo_image_surface_create_from_png("关闭按钮1.png");
    surfaceclose52 = cairo_image_surface_create_from_png("关闭按钮2.png");
    surfaceclose53 = cairo_image_surface_create_from_png("关闭按钮3.png");

    background1 = gtk_image_new_from_surface(surfacemainbackgroud);
    search = gtk_image_new_from_surface(surfaceresearch);
    friend = gtk_image_new_from_surface(surfacefriendimage);
    closebut = gtk_image_new_from_surface(surfaceclose51);


}

static void loadinfo()
{
    GtkWidget *userid;
    userid = gtk_label_new(CurrentUserInfo->nickName);
    //设置字体大小
    PangoFontDescription *font;
    font = pango_font_description_from_string("Sans");//"Sans"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(userid, font);

    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    //加载用户头像
    int finduidflag = 0;
    FriendInfo *rear = FriendInfoHead;
    while (rear)
    {
        if (rear->user.uid == CurrentUserInfo->uid)
        {
            finduidflag = 1;
            break;
        }
        rear = rear->next;
    }
    if (finduidflag == 1)
    {
        char userhead[80] = {0};
        static cairo_t *cr;
        cairo_surface_t *surface;

        sprintf(userhead, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, CurrentUserInfo->uid);
        //加载一个图片
        surface = cairo_image_surface_create_from_png(userhead);
        int w = cairo_image_surface_get_width(surface);
        int h = cairo_image_surface_get_height(surface);
        //创建画布
        surfacehead2 = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 125, 125);
        //创建画笔
        cr = cairo_create(surfacehead2);
        //缩放
        cairo_arc(cr, 60, 60, 60, 0, M_PI * 2);
        cairo_clip(cr);
        cairo_scale(cr, 125.0 / w, 126.0 / h);
        //把画笔和图片相结合。
        cairo_set_source_surface(cr, surface, 0, 0);
        cairo_paint(cr);
        headx = gtk_image_new_from_surface(surfacehead2);
        cairo_destroy(cr);
    }
}

static void
destroy_surfaces()
{
    g_print("destroying surfaces2");
    cairo_surface_destroy(surfacemainbackgroud);
    cairo_surface_destroy(surfacehead2);
    cairo_surface_destroy(surfaceresearch);
    cairo_surface_destroy(surfacefriendimage);

}

//单击分组显示右键菜单
gboolean button2_press_event2(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GdkEventButton *event_button;
    GtkWidget *menu = GTK_WIDGET(data);
    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iter);

    if (event->type == GDK_BUTTON_PRESS)
    {
        int i;
        GtkTreePath *path;
        path = gtk_tree_model_get_path(model, &iter);
        i = gtk_tree_path_get_indices(path)[0];

        event_button = (GdkEventButton *) event;

        if (event->button == 0x1)
        {
            return FALSE;
        }
        if (event->button == 0x2)
        {
            return FALSE;
        }
        if (event->button == 0x3)
        {
            if ((gtk_tree_model_iter_has_child(model, &iter)) || (friends->groups[i].friendCount == 0))
            {
                gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event_button->button, event_button->time);
                return FALSE;
            }
        }
    }

    return FALSE;
}

//树状视图双击列表事件 &&单击好友显示右键菜单
gboolean button2_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GdkEventButton *event_button;
    GtkTreeIter iter;

    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iter);//拿到它iter
    GtkWidget *menu = GTK_WIDGET(data);
    if (event->type == GDK_BUTTON_PRESS)
    {
        event_button = (GdkEventButton *) event;

        if (event->button == 0x1)
        {
            return FALSE;
        }
        if (event->button == 0x2)
        {
            return FALSE;
        }
        if (event->button == 0x3)
        {
            int i, j;
            GtkTreePath *path;
            path = gtk_tree_model_get_path(model, &iter);
            i = gtk_tree_path_get_indices(path)[0];
            j = gtk_tree_path_get_indices(path)[1];

            if ((gtk_tree_model_iter_has_child(model,
                    &iter) == 0) && !((i == 0) && (j == 0)) && (friends->groups[i].friendCount > 0))
            {
                gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event_button->button, event_button->time);
                return FALSE;
            }
        }
    }
    else if (event->type == GDK_2BUTTON_PRESS && event->button == 0x1)
    {
        int i, j;
        int uidfindflag = 0;
        GtkTreePath *path;
        FriendInfo *friendinforear;
        path = gtk_tree_model_get_path(model, &iter);
        i = gtk_tree_path_get_indices(path)[0];
        j = gtk_tree_path_get_indices(path)[1];

        if (gtk_tree_model_iter_has_child(model, &iter) == 0)
        {
            uint32_t t;
            gtk_tree_model_get(model, &iter, FRIENDUID_COL, &t, -1);
            if (t == CurrentUserInfo->uid)
            {
                return FALSE;
            }
            friendinforear = FriendInfoHead;
            while (friendinforear)
            {
                if (friendinforear->user.uid == t)
                {

                    uidfindflag = 1;
                    break;
                }
                else
                {
                    friendinforear = friendinforear->next;
                }
            }
            if (uidfindflag == 1)
            {
                if (friendinforear->chartwindow == NULL)
                {
                    MainChart(friendinforear);
                }
                else
                {
                    gtk_window_set_keep_above(GTK_WINDOW(friendinforear->chartwindow), TRUE);
                }
            }

        }
    }
    return FALSE;

}


int deal_with_recv_message(void *data)  //图片处理函数
{
    struct RECVImageMessagedata *recv_message = (struct RECVImageMessagedata *) data;
    recv_message->imagecount--;
    if (recv_message->imagecount == 0)
    {
        ShoweRmoteText(recv_message->message_data, recv_message->userinfo,
                recv_message->charlen);
        free(recv_message->message_data);
        free(recv_message);
    }
    return FALSE;
}

int image_message_recv(gchar *recv_text, FriendInfo *info, int charlen)
{
    int i = 0;
    int isimageflag = 0;
    struct RECVImageMessagedata *image_message_data
            = (struct RECVImageMessagedata *) malloc(sizeof(struct RECVImageMessagedata));
    gchar *message_recv = (gchar *) malloc(charlen);
    memcpy(message_recv, recv_text, charlen);
    image_message_data->imagecount = 0;
    image_message_data->message_data = message_recv;
    image_message_data->userinfo = info;
    image_message_data->charlen = charlen;
    while (i < charlen)
    {
        if (recv_text[i] != '\0')
        {
            i++;
        }
        else
        {

            switch (recv_text[i + 1])
            {
                case 1:
                {
                    i++;
                    while (recv_text[i] != '\0')
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

                    isimageflag = 1;
                    char filename[256] = {0};
                    char strdest[17] = {0};
                    i += 2;
                    image_message_data->imagecount++;
                    memcpy(strdest, &recv_text[i], 16);
                    HexadecimalConversion(filename, strdest); //进制转换，将MD5值的字节流转换成十六进制
                    FindImage(strdest, image_message_data, deal_with_recv_message); //请求图片
                    i = i + 16;
                    break;
                }
                default:
                {
//                        i += 2;
                    break;
                };
            }
        }

    }
    if (isimageflag == 0)
    {
        ShoweRmoteText(image_message_data->message_data, image_message_data->userinfo,
                image_message_data->charlen);
        free(message_recv);
        return 0;
    }

    return 1;
}

void RecdServerMsg(const gchar *rcvd_text, uint16_t len, uint32_t recd_uid)
{

    log_info("DEBUG", "Recv Message.From %u,Text:%s\n", recd_uid, rcvd_text);
    int uidfindflag = 0;
    FriendInfo *userinfo = FriendInfoHead;
    while (userinfo)
    {
        if (userinfo->user.uid == recd_uid)
        {
            uidfindflag = 1;
            break;
        }
        else
        {
            userinfo = userinfo->next;
        }
    }
    if (uidfindflag == 1)
    {
        if (userinfo->chartwindow == NULL)
        {
            MainChart(userinfo);
        }
        else
        {
            gtk_window_present(GTK_WINDOW(userinfo->chartwindow));
        }
        image_message_recv(rcvd_text, userinfo, len);

    }
}

static gint background_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    //设置在非按钮区域内移动窗口
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                event->x_root, event->y_root, event->time);
    }
    return 0;

}

//鼠标点击事件
static gint closebut_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {              //设置关闭按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose52); //置换图标
    }
    return 0;
}

//鼠标抬起事件
static gint closebut_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

//    x = event->x;  // 取得鼠标相对于窗口的位置
//    y = event->y;
    if (event->button == 1)       // 判断是否是点击关闭图标

    {
        gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose51);  //设置关闭按钮
        destroy_surfaces();
        DeleteEvent();
    }

    return 0;
}

//鼠标移动事件
static gint closebut_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose53);
    return 0;
}

static gint closebut_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)         // 离开事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) closebut, surfaceclose51);
    return 0;
}

//头像
//鼠标点击事件
static gint headx_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
        //gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave1); //置换图标
    }
    return 0;
}

//头像
//鼠标抬起事件
static gint headx_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 1)
    {
        FriendInfo *friendinforear;
        friendinforear = FriendInfoHead;
        while (friendinforear)
        {
            if ((friendinforear->user.uid == CurrentUserInfo->uid) && friendinforear->Infowind == NULL)
            {
                //查看资料
                OnlyLookInfo(friendinforear);
                break;
            }
            else
            {
                friendinforear = friendinforear->next;
            }
        }
    }
    return 0;
}

//头像
//鼠标移动事件
static gint headx_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    return 0;
}

//头像
//鼠标离开事件
static gint headx_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    //gtk_image_set_from_surface((GtkImage *) Infosave, Surfacesave);
    return 0;
}

//右键菜单发送即时消息
static gint sendmsg_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iter);
    int i, j;
    int uidfindflag = 0;
    GtkTreePath *path;
    FriendInfo *friendinforear;
    path = gtk_tree_model_get_path(model, &iter);
    i = gtk_tree_path_get_indices(path)[0];
    j = gtk_tree_path_get_indices(path)[1];

    if (gtk_tree_model_iter_has_child(model,
            &iter) == 0 && ((i == 0 && j > 0) || ((i != 0) && (friends->groups[i].friendCount > 0))))
    {
        uint32_t t;
        gtk_tree_model_get(model, &iter, FRIENDUID_COL, &t, -1);
        friendinforear = FriendInfoHead;
        while (friendinforear)
        {
            if (friendinforear->user.uid == t)
            {

                uidfindflag = 1;
                break;
            }
            else
            {
                friendinforear = friendinforear->next;
            }
        }
        if (uidfindflag == 1)
        {
            if (friendinforear->chartwindow == NULL)
            {
                MainChart(friendinforear);
            }
            else
            {
                gtk_window_set_keep_above(GTK_WINDOW(friendinforear->chartwindow), TRUE);
            }
        }
    }
    return 0;
}

//右键菜单查看好友资料
static gint lookinfo_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iter);
    int i, j;
    int uidfindflag = 0;
    GtkTreePath *path;
    FriendInfo *friendinforear;
    path = gtk_tree_model_get_path(model, &iter);
    i = gtk_tree_path_get_indices(path)[0];
    j = gtk_tree_path_get_indices(path)[1];

    if (gtk_tree_model_iter_has_child(model,
            &iter) == 0 && ((i == 0 && j > 0) || ((i != 0) && (friends->groups[i].friendCount > 0))))
    {
        uint32_t t;
        gtk_tree_model_get(model, &iter, FRIENDUID_COL, &t, -1);
        friendinforear = FriendInfoHead;
        while (friendinforear)
        {
            if (friendinforear->user.uid == t)
            {
                uidfindflag = 1;
                break;
            }
            else
            {
                friendinforear = friendinforear->next;
            }
        }
        if (uidfindflag == 1)
        {
            if (friendinforear->Infowind == NULL)
            {
                OnlyLookInfo(friendinforear);
            }
            else
            {
                gtk_window_set_keep_above(GTK_WINDOW(friendinforear->Infowind), TRUE);
            }
        }
    }
    return 0;
}

static gint search_button_release_event(GtkWidget *widget, GdkEventButton *event,

        gpointer data)
{

    if (AddFriendflag)//判断是否打开搜索窗口
    {
        AddFriendFun();
        //Friend_Fequest_Popup(10001,"as");//添加爱弹出框

    } //调用添加好友函数
    return 0;
}

int MainInterFace()
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;//列表
    vbox = gtk_box_new(TRUE, 5);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);//固定窗口大小
    gtk_widget_set_size_request(GTK_WIDGET(window), 284, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    MainLayout = gtk_fixed_new();
    frameLayout = gtk_layout_new(NULL, NULL);

    create_surfaces();

    background_event_box = BuildEventBox(background1,
            G_CALLBACK(background_button_press_event),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    closebut_event_box = BuildEventBox(closebut,
            G_CALLBACK(closebut_button_press_event),
            G_CALLBACK(closebut_enter_notify_event),
            G_CALLBACK(closebut_leave_notify_event),
            G_CALLBACK(closebut_button_release_event),
            NULL,
            NULL);

    search = gtk_image_new_from_surface(surfaceresearch);
    search_event_box = BuildEventBox(search, NULL, NULL, NULL, G_CALLBACK(search_button_release_event), NULL, NULL);


    gtk_fixed_put(GTK_FIXED(MainLayout), background_event_box, 0, 0);//起始坐标
    gtk_fixed_put(GTK_FIXED(MainLayout), closebut_event_box, 247, 0);
    gtk_fixed_put(GTK_FIXED(MainLayout), search_event_box, 0, 140);
    gtk_fixed_put(GTK_FIXED(MainLayout), friend, 1, 178);
    loadinfo();

    headx_event_box = BuildEventBox(headx,
            G_CALLBACK(headx_button_press_event),
            G_CALLBACK(headx_enter_notify_event),
            G_CALLBACK(headx_leave_notify_event),
            G_CALLBACK(headx_button_release_event),
            NULL,
            NULL);
    gtk_fixed_put(GTK_FIXED(MainLayout), headx_event_box, 10, 15);


    gtk_container_add(GTK_CONTAINER(window), frameLayout);//frameLayout 加入到window
    gtk_container_add(GTK_CONTAINER(frameLayout), MainLayout);

    treeView = (GtkTreeView *) gtk_tree_view_new_with_model(createModel());//list
    //gtk_tree_view_column_set_resizable(column,TRUE);//加了就bug了
    gtk_tree_view_set_headers_visible(treeView, 0);//去掉头部空白

    //添加树形视图
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes(NULL, renderer,
            "pixbuf", PIXBUF_COL,
            NULL);
    //gtk_tree_view_column_set_sort_column_id(column, 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW (treeView), column);
    gtk_tree_view_column_set_resizable(column, TRUE);


    //添加滚动条

    GtkScrolledWindow *sw = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
    //设置滚动条常在状态
    gtk_scrolled_window_set_policy(sw,
            GTK_POLICY_ALWAYS,
            GTK_POLICY_ALWAYS);
    //获取水平滚动条
    GtkWidget *widget = gtk_scrolled_window_get_hscrollbar(sw);
    gtk_container_add(GTK_CONTAINER(sw), (GtkWidget *) treeView);
    gtk_fixed_put(GTK_FIXED(MainLayout), (GtkWidget *) sw, 0, 225);
    gtk_widget_set_size_request((GtkWidget *) sw, 284, 358);
//
//    g_signal_connect(G_OBJECT(treeView), "button_press_event",
//            G_CALLBACK(button_press_event), treeView);
//
    //    //右键菜单
    GtkWidget *menu1, *menu2;
    GtkWidget *add;
    GtkWidget *delete;
    GtkWidget *rename;
    GtkWidget *addpeople;
    GtkWidget *Refresh;
    GtkWidget *sendmsg;
    GtkWidget *deletefriend;
    GtkWidget *remark;
    GtkWidget *sendfile;
    GtkWidget *lookinfo;
    //分组菜单
    menu1 = gtk_menu_new();
    add = gtk_menu_item_new_with_mnemonic("添加分组");
    gtk_container_add(GTK_CONTAINER(menu1), add);
    gtk_widget_show(add);
    delete = gtk_menu_item_new_with_mnemonic("删除分组");
    gtk_container_add(GTK_CONTAINER(menu1), delete);
    gtk_widget_show(delete);
    rename = gtk_menu_item_new_with_mnemonic("重命名分组");
    gtk_container_add(GTK_CONTAINER(menu1), rename);
    gtk_widget_show(rename);

    addpeople = gtk_menu_item_new_with_mnemonic("添加联系人");
    gtk_container_add(GTK_CONTAINER(menu1), addpeople);
    gtk_widget_show(addpeople);
    Refresh = gtk_menu_item_new_with_mnemonic("刷新好友列表");
    gtk_container_add(GTK_CONTAINER(menu1), Refresh);
    gtk_widget_show(Refresh);

    g_signal_connect(G_OBJECT(treeView), "button_press_event",
            G_CALLBACK(button2_press_event2), (gpointer) menu1);
    //添加分组事件
    g_signal_connect(G_OBJECT(add), "button_release_event",
                     G_CALLBACK(AddGroupButtonPressEvent), (gpointer) menu1);

    //删除分组事件
    g_signal_connect(G_OBJECT(delete), "button_press_event",
                     G_CALLBACK(DeleteGroupButtonPressEvent), treeView);
    //重命名分组事件
    g_signal_connect(G_OBJECT(rename), "button_press_event",
                     G_CALLBACK(RenameGroupButtonPressEvent), treeView);
    //添加好友
    g_signal_connect(G_OBJECT(addpeople), "button_release_event",
                     G_CALLBACK(search_button_release_event), treeView);

    //好友菜单
    menu2 = gtk_menu_new();
    sendmsg = gtk_menu_item_new_with_mnemonic("发送即时消息");
    gtk_container_add(GTK_CONTAINER(menu2), sendmsg);
    gtk_widget_show(sendmsg);
    deletefriend = gtk_menu_item_new_with_mnemonic("删除好友");
    gtk_container_add(GTK_CONTAINER(menu2), deletefriend);
    gtk_widget_show(deletefriend);
    remark = gtk_menu_item_new_with_mnemonic("修改备注");
    gtk_container_add(GTK_CONTAINER(menu2), remark);
    gtk_widget_show(remark);
    sendfile = gtk_menu_item_new_with_mnemonic("发送文件");
    gtk_container_add(GTK_CONTAINER(menu2), sendfile);
    gtk_widget_show(sendfile);
    lookinfo = gtk_menu_item_new_with_mnemonic("查看资料");
    gtk_container_add(GTK_CONTAINER(menu2), lookinfo);
    gtk_widget_show(lookinfo);

    g_signal_connect(G_OBJECT(treeView), "button_press_event",
            G_CALLBACK(button2_press_event), (gpointer) menu2);

    g_signal_connect(G_OBJECT(sendmsg), "button_press_event",
            G_CALLBACK(sendmsg_button_press_event), (gpointer) treeView);

    g_signal_connect(G_OBJECT(lookinfo), "button_press_event",
            G_CALLBACK(lookinfo_button_press_event), (gpointer) treeView);


    gtk_widget_show_all(window);
    //隐藏水平滚动条
    gtk_widget_hide(widget);
    //gtk_main();
    return 0;
}

gboolean DestoryMainInterFace(gpointer user_data)
{
    gtk_widget_destroy(window);
    return FALSE;
}