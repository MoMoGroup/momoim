#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <logger.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "chartmessage.h"
#include "onlylookinfo.h"
#include "managegroup/ManageGroup.h"
#include"manage_friend/friend.h"
#include "SetupWind.h"


static GtkWidget *status;

static GtkWidget *background1, *search, *friend, *change, *closebut, *SetUp;
static GtkWidget *window;
static GtkWidget *frameLayout, *MainLayout;
static cairo_surface_t *surfacechangetheme, *surfacechangetheme2, *surfacemainbackgroud, *surfacehead2, *surfaceresearch, *surfacefriendimage, *surfaceclose51, *surfaceclose52, *surfaceclose53;
GtkWidget *userid, *headx;
//全局变量用以实时更新昵称和头像
int MarkNewpasswd = 0, MarkUpdateInfo = 0;
GtkTreeView *treeView;

static cairo_surface_t *surfacesetup;

GtkTreeStore *TreeViewListStore;
static GdkPixbuf *pixbuf;
static cairo_t *cr;
static GtkWidget *vbox;
static GtkEventBox *closebut_event_box, *background_event_box, *search_event_box, *headx_event_box, *change_event_box, *setup_event_box;
static GtkWidget *friend_mov_group;
cairo_surface_t *surface_status, *surface_status2;

//换肤变量
static GtkWidget *huanfuwindow;
static GtkWidget *huanfuLayout;
static GtkWidget *iback, *isure, *icancel, *ipic1, *ipic2, *ipic3;
static cairo_surface_t *sbackground, *ssure1, *ssure2, *scancel1, *scancel2, *spic11, *spic12, *spic21, *spic22, *spic31, *spic32;
static GtkEventBox *sure_event_box, *cancel_event_box, *ipic1_event_box, *ipic2_event_box, *ipic3_event_box;
int FlagChange = 1;

/**********换肤窗口********/

//按下确定
static gint sure_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                    gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));//设置光标
        gtk_image_set_from_surface((GtkImage *) isure, ssure2);//置换图片
    }

    return 0;
}

//离开确定按钮
static gint sure_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)
{
    if (event->button == 1)
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));//设置光标
        gtk_image_set_from_surface((GtkImage *) isure, ssure2);//置换图片

        if (FlagChange == 1)
        {
            //换肤成cartoon
            char mulu_benji[80], mulu_thempath[80], mulu_themnewpath[80];
            sprintf(mulu_benji, "%s/.momo", getpwuid(getuid())->pw_dir);//获取本机主目录
            sprintf(mulu_thempath, "%s/current_theme", mulu_benji);
            unlink(mulu_thempath);
            sprintf(mulu_themnewpath, "theme/cartoon/");
            symlink(mulu_themnewpath, mulu_thempath);

            DestoryMainInterface();
            MainInterFace();
        }

        if (FlagChange == 2)
        {
            //换肤成flower
            char mulu_benji[80], mulu_thempath[80], mulu_themnewpath[80];
            sprintf(mulu_benji, "%s/.momo", getpwuid(getuid())->pw_dir);//获取本机主目录
            sprintf(mulu_thempath, "%s/current_theme", mulu_benji);
            unlink(mulu_thempath);
            sprintf(mulu_themnewpath, "theme/flower/");
            symlink(mulu_themnewpath, mulu_thempath);

            DestoryMainInterface();
            MainInterFace();
        }

        if (FlagChange == 3)
        {
            //换肤成lol
            char mulu_benji[80], mulu_thempath[80], mulu_themnewpath[80];
            sprintf(mulu_benji, "%s/.momo", getpwuid(getuid())->pw_dir);//获取本机主目录
            sprintf(mulu_thempath, "%s/current_theme", mulu_benji);
            unlink(mulu_thempath);
            sprintf(mulu_themnewpath, "theme/lol/");
            symlink(mulu_themnewpath, mulu_thempath);

            DestoryMainInterface();
            MainInterFace();
        }
        gtk_widget_destroy(huanfuwindow);
    }

    return 0;
}


//按下取消按钮
static gint cancel_button_press_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));//设置光标
        gtk_image_set_from_surface((GtkImage *) icancel, scancel2);//置换图片
    }

    return 0;
}

//离开取消按钮
static gint cancel_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    if (event->button == 1)
    {
        //  gtk_image_set_from_surface((GtkImage *) icancel, scancel2);
        gtk_widget_destroy(huanfuwindow);
    }

    return 0;
}

//抬起 皮肤1
static gint ipic1_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    if (event->button == 1)
    {
        gtk_image_set_from_surface((GtkImage *) ipic1, spic12);
        gtk_image_set_from_surface((GtkImage *) ipic2, spic21);
        gtk_image_set_from_surface((GtkImage *) ipic3, spic31);
        FlagChange = 1;
    }

    return 0;
}

//抬起 皮肤2
static gint ipic2_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    if (event->button == 1)
    {
        gtk_image_set_from_surface((GtkImage *) ipic2, spic22);
        gtk_image_set_from_surface((GtkImage *) ipic1, spic11);
        gtk_image_set_from_surface((GtkImage *) ipic3, spic31);
        FlagChange = 2;
    }

    return 0;
}

//抬起 皮肤3
static gint ipic3_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    if (event->button == 1)
    {
        gtk_image_set_from_surface((GtkImage *) ipic3, spic32);
        gtk_image_set_from_surface((GtkImage *) ipic1, spic11);
        gtk_image_set_from_surface((GtkImage *) ipic2, spic21);
        FlagChange = 3;
    }

    return 0;
}

//主函数
void changethemeface()
{
    //初始化窗口
    huanfuwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(huanfuwindow), GTK_WIN_POS_CENTER);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (huanfuwindow), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(huanfuwindow), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(huanfuwindow), 432, 238);
    huanfuLayout = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER (huanfuwindow), huanfuLayout);


    //取图片
    sbackground = ChangeThem_png("换肤背景.png");
    ssure1 = ChangeThem_png("确定.png");
    ssure2 = ChangeThem_png("确定2.png");
    scancel1 = ChangeThem_png("资料取消.png");
    scancel2 = ChangeThem_png("资料取消2.png");
    spic11 = ChangeThem_png("卡通.png");
    spic12 = ChangeThem_png("卡通2.png");
    spic21 = ChangeThem_png("小清新.png");
    spic22 = ChangeThem_png("小清新2.png");
    spic31 = ChangeThem_png("LOL.png");
    spic32 = ChangeThem_png("LOL2.png");


    iback = gtk_image_new_from_surface(sbackground);
    isure = gtk_image_new_from_surface(ssure1);
    icancel = gtk_image_new_from_surface(scancel1);
    ipic1 = gtk_image_new_from_surface(spic11);
    ipic2 = gtk_image_new_from_surface(spic21);
    ipic3 = gtk_image_new_from_surface(spic31);

    gtk_fixed_put(GTK_FIXED(huanfuLayout), iback, 0, 0);//起始坐标

    //事件盒子
    ipic1_event_box = BuildEventBox(ipic1,
                                    NULL,
                                    NULL,
                                    NULL,
                                    G_CALLBACK(ipic1_button_release_event),
                                    NULL,
                                    NULL);

    ipic2_event_box = BuildEventBox(ipic2,
                                    NULL,
                                    NULL,
                                    NULL,
                                    G_CALLBACK(ipic2_button_release_event),
                                    NULL,
                                    NULL);

    ipic3_event_box = BuildEventBox(ipic3,
                                    NULL,
                                    NULL,
                                    NULL,
                                    G_CALLBACK(ipic3_button_release_event),
                                    NULL,
                                    NULL);

    sure_event_box = BuildEventBox(isure,
                                   G_CALLBACK(sure_button_press_event),
                                   NULL,
                                   NULL,
                                   G_CALLBACK(sure_button_release_event),
                                   NULL,
                                   NULL);

    cancel_event_box = BuildEventBox(icancel,
                                     G_CALLBACK(cancel_button_press_event),
                                     NULL,
                                     NULL,
                                     G_CALLBACK(cancel_button_release_event),
                                     NULL,
                                     NULL);

    //布局
    gtk_widget_set_size_request(GTK_WIDGET(iback), 432, 238);
    gtk_fixed_put(GTK_FIXED(huanfuLayout), GTK_WIDGET(ipic1_event_box), 20, 70);
    gtk_fixed_put(GTK_FIXED(huanfuLayout), GTK_WIDGET(ipic2_event_box), 155, 70);
    gtk_fixed_put(GTK_FIXED(huanfuLayout), GTK_WIDGET(ipic3_event_box), 290, 70);
    gtk_fixed_put(GTK_FIXED(huanfuLayout), GTK_WIDGET(sure_event_box), 240, 205);
    gtk_fixed_put(GTK_FIXED(huanfuLayout), GTK_WIDGET(cancel_event_box), 340, 205);


    gtk_widget_show_all(huanfuwindow);

}

/***********换肤END*************/

static gint friendListStoreFunc(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
    int64_t priA, priB;
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), a, PRIORITY_COL, &priA, -1);
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), b, PRIORITY_COL, &priB, -1);
    return priA - priB;
}

GtkTreeModel *createModel()
{
    int64_t priority;
    gint i, j;
    cairo_surface_t *surface;

    GtkTreeIter iter1, iter2;

    TreeViewListStore = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_INT64);
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(TreeViewListStore), friendListStoreFunc, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(TreeViewListStore),
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
            //char friendname[20] = {0};
            char mulu[80] = {0};
            sprintf(mulu, "%s/.momo/friend/%u.png", getpwuid(getuid())->pw_dir, friends->groups[i].friends[j]);
            pixbuf = gdk_pixbuf_new_from_file(mulu, NULL);

            FriendInfo *rear = FriendInfoHead;

            while (rear)
            {
                if (rear->uid == friends->groups[i].friends[j])
                {
                    // memcpy(friendname, rear->user.nickName, sizeof(rear->user.nickName));
                    break;
                }
                rear = rear->next;
            }
            if (!rear)
            {
                continue;
            }

            pixbuf = DrawFriend(&rear->user, rear->isonline);

            gtk_tree_store_append(TreeViewListStore, &iter2, &iter1);//
            if (CurrentUserInfo->uid == friends->groups[i].friends[j])//说明是自己
            {

                priority = 5256000;//10年
            }
            else if (rear->isonline)
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
                               -1);
            g_object_unref(pixbuf);

        }
    }

    return GTK_TREE_MODEL(TreeViewListStore);
}


static void create_surfaces()
{

    surfacemainbackgroud = ChangeThem_png("主背景.png");
    surfaceresearch = ChangeThem_png("搜索.png");
    surfacefriendimage = ChangeThem_png("好友.png");
    surfacechangetheme = ChangeThem_png("换肤.png");
    surfacechangetheme2 = ChangeThem_png("换肤.png");
    surfaceclose51 = ChangeThem_png("关闭按钮1.png");
    surfaceclose52 = ChangeThem_png("关闭按钮2.png");
    surfaceclose53 = ChangeThem_png("关闭按钮3.png");
    surfacesetup = ChangeThem_png("设置图标.png");

    background1 = gtk_image_new_from_surface(surfacemainbackgroud);
    search = gtk_image_new_from_surface(surfaceresearch);
    change = gtk_image_new_from_surface(surfacechangetheme);
    friend = gtk_image_new_from_surface(surfacefriendimage);
    closebut = gtk_image_new_from_surface(surfaceclose51);
    SetUp = gtk_image_new_from_surface(surfacesetup);


}

GtkWidget *StatusShowText;

static void loadinfo()
{
    userid = gtk_label_new(CurrentUserInfo->nickName);
    //设置字体大小
    PangoFontDescription *font;
    font = pango_font_description_from_string("Droid Sans Mono");//"Droid Sans Mono"字体名
    pango_font_description_set_size(font, 20 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(userid, font);

    gtk_fixed_put(GTK_FIXED(MainLayout), userid, 170, 90);

    StatusShowText = gtk_label_new("");
    pango_font_description_set_size(font, 15 * PANGO_SCALE);//设置字体大小
    gtk_widget_override_font(StatusShowText, font);
    ShowStatus("在线");
    gtk_fixed_put(GTK_FIXED(MainLayout), StatusShowText, 140, 65);


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
        HexadecimalConversion(userhead, CurrentUserInfo->icon);
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
    cairo_surface_destroy(surfacechangetheme);
    cairo_surface_destroy(surfacechangetheme2);
    cairo_surface_destroy(surfacesetup);

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

gboolean button2_dblclick_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_2BUTTON_PRESS && event->button == 0x1)
    {
        GtkTreeIter iter;
        GtkTreeView *treeview = GTK_TREE_VIEW(widget);
        GtkTreeModel *model = gtk_tree_view_get_model(treeview);
        GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
        gtk_tree_selection_get_selected(selection, &model, &iter);//拿到它iter

        GtkTreePath *path;
        path = gtk_tree_model_get_path(model, &iter);

        uint32_t id = 0;
        gtk_tree_model_get(model, &iter, FRIENDUID_COL, &id, -1);
        int uidfindflag = 0;
        FriendInfo *friendinforear;

        if (id >= 10000)
        {
            if (id == CurrentUserInfo->uid)
            {
                return FALSE;
            }
            friendinforear = FriendInfoHead;
            while (friendinforear)
            {
                if (friendinforear->user.uid == id)
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
                    gtk_window_present(GTK_WINDOW(friendinforear->chartwindow));
                }
            }

        }
        else//双击的是分组
        {
            if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(TreeViewListStore), &iter))//如果有子行，展开
            {
                if (gtk_tree_view_expand_row(treeview, path, FALSE))
                {
                    //展开本行成功
                    g_print("expand child ");
                }
                else //本行已经被展开
                {
                    //收起本行
                    gtk_tree_view_collapse_row(treeview, path);
                }
            }


        }
    }
    return FALSE;
}

//树状视图双击列表事件 &&单击好友显示右键菜单
gboolean button2_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);


    if (event->button == 0x3)
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
        gtk_tree_selection_get_selected(selection, &model, &iter);//拿到 选中列的iter
        uint32_t id = 0;
        gtk_tree_model_get(model, &iter, FRIENDUID_COL, &id, -1);

        if (id == CurrentUserInfo->uid)
        {
            return FALSE;
        }
        else if (id >= 256)
        {
            GtkMenu *menu = g_object_get_data(G_OBJECT(widget), "FriendMenu");
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(friend_mov_group), MovFriendButtonEvent(treeView));
            gtk_menu_popup(menu, NULL, NULL, NULL, NULL, event->button, event->time);
        }

        else
        {
            GtkMenu *menu = g_object_get_data(G_OBJECT(widget), "GroupMenu");
            gtk_menu_popup(menu, NULL, NULL, NULL, NULL, event->button, event->time);
        }
    }
    return FALSE;

}


gboolean recv_progress_bar_crcle(void *data)
{
    struct RECVFileMessagedata *recv_file_bar_crcle = (struct RECVFileMessagedata *) data;

    if (recv_file_bar_crcle->file_loading_end == 0)
    {
        gdouble pvalue;
        pvalue = (gdouble) recv_file_bar_crcle->file_count / (gdouble) recv_file_bar_crcle->file_size;
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(recv_file_bar_crcle->progressbar), pvalue);
        return 1;
    }
    else
    {
        gtk_widget_destroy(recv_file_bar_crcle->file);
        gtk_widget_destroy(recv_file_bar_crcle->progressbar);
        gchar filemulu[200] = {0};
        sprintf(filemulu, "文件保存地址为%s", recv_file_bar_crcle->filemulu);
        ShoweRmoteText(filemulu, recv_file_bar_crcle->userinfo,
                       strlen(filemulu));
        free(recv_file_bar_crcle->filename);
        free(recv_file_bar_crcle);
        return 0;
    }
}

int deal_with_recv_file(CRPBaseHeader *header, void *data)
{
    struct RECVFileMessagedata *recv_message = (struct RECVFileMessagedata *) data;
    int ret = 1;
    switch (header->packetID)
    {
        case CRP_PACKET_FAILURE:
        {
            CRPPacketFailure *infodata = CRPFailureCast(header);
            log_info("FAILURE reason", infodata->reason);
            fclose(recv_message->Wfp);
            recv_message->file_loading_end = 1;
            if ((void *) infodata != header->data)
            {
                free(infodata);
            }
            return 0;
        };
        case  CRP_PACKET_FILE_DATA_START:
        {
            log_info("Recv Message", "Packet id :%d,SessionID:%d\n", header->packetID, header->sessionID);
            CRPOKSend(sockfd, header->sessionID);
            break;
        };
        case CRP_PACKET_FILE_DATA:
        {

            CRPPacketFileData *packet = CRPFileDataCast(header);
            fwrite(packet->data, 1, packet->length, recv_message->Wfp);
            recv_message->file_count = recv_message->file_count + packet->length;
            CRPOKSend(sockfd, header->sessionID);
            if ((void *) packet != header->data)
            {
                free(packet);
            }
            break;
        };
        case CRP_PACKET_FILE_DATA_END:
        {
            fclose(recv_message->Wfp);
            recv_message->file_loading_end = 1;
            ret = 0;
            break;
        };

    }

    return ret;
}

//接收文件处理函数
int file_message_recv(const gchar *recv_text, FriendInfo *info, int charlen)
{
    if (info->chartwindow != NULL)
    {
        GtkWidget *dialog;
        struct RECVFileMessagedata *file_message_data = (struct RECVFileMessagedata *) malloc(sizeof(struct RECVFileMessagedata));
        file_message_data->charlen = charlen;
        file_message_data->filename = (gchar *) malloc(100);
        memcpy(file_message_data->filename, recv_text, charlen - 20); //获取文件名
        file_message_data->filename[charlen - 20] = '\0';
        gchar file_info[256];
        sprintf(file_info, "莫默询问您：\n您想接收 %s 这份文件吗？", file_message_data->filename);

        dialog = gtk_message_dialog_new(GTK_WINDOW(info->chartwindow), GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
                                        file_info);
        gtk_window_set_title(GTK_WINDOW (dialog), "Question");
        gint result = gtk_dialog_run(GTK_DIALOG (dialog));
        if (result == -5)
        {
            gtk_widget_destroy(dialog);
            //文件的信息初始化
            unsigned char strdest[17] = {0};
            size_t filename_len = strlen(file_message_data->filename);
            memcpy(&file_message_data->file_size, recv_text + filename_len, 4);//获取文件大小
            memcpy(strdest, recv_text + filename_len + 4, 16);
            file_message_data->file_loading_end = 0;
            file_message_data->file_count = 0;
            file_message_data->userinfo = info;
            gchar sendfile_size[100];
            PangoFontDescription *font;
            session_id_t session_id;
            //写文件
            GtkWidget *save_dialog;
            save_dialog = gtk_file_chooser_dialog_new("将文件保存在...", GTK_WINDOW(info->chartwindow),
                                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                      NULL);
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(save_dialog), file_message_data->filename);
            gint save_result = gtk_dialog_run(GTK_DIALOG (save_dialog));
            if (save_result == GTK_RESPONSE_ACCEPT)
            {
                gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (save_dialog));
                size_t len_filename = strlen(filename);
                memcpy(file_message_data->filemulu, filename, len_filename);
                file_message_data->filemulu[len_filename] = 0;
                g_free(filename);
                gtk_widget_destroy(save_dialog);

                if (file_message_data->file_size / 1048576.0 > 0)
                {
                    sprintf(sendfile_size, "\t %s \n 大小为：%.2f M", file_message_data->filename,
                            file_message_data->file_size / 1048576.0);
                }
                else
                {
                    sprintf(sendfile_size,
                            "\t %s \n 大小为：%d byte", file_message_data->filename, file_message_data->file_size);
                }
                //显示的文件名和大小
                file_message_data->file = gtk_label_new(sendfile_size);
                font = pango_font_description_from_string("Droid Sans Mono");//"Droid Sans Mono"字体名
                pango_font_description_set_size(font, 10 * PANGO_SCALE);//设置字体大小
                gtk_widget_override_font(file_message_data->file, font);
                gtk_fixed_put(GTK_FIXED(info->chartlayout), file_message_data->file, 160, 5);
                gtk_widget_show(file_message_data->file);                   //文件名和大小

                //进度条
                file_message_data->progressbar = gtk_progress_bar_new();        //进度条
                gtk_fixed_put(GTK_FIXED(info->chartlayout), file_message_data->progressbar, 175, 50);
                gtk_widget_show(file_message_data->progressbar);
                g_idle_add(recv_progress_bar_crcle, file_message_data);  //用来更新进度条


                file_message_data->Wfp = (fopen(file_message_data->filemulu, "w"));

                session_id = CountSessionId();
                AddMessageNode(session_id, deal_with_recv_file, file_message_data);
                CRPFileRequestSend(sockfd, session_id, 0, strdest);

            }
            else
            {
                free(file_message_data->filename);
                free(file_message_data);
                gtk_widget_destroy(save_dialog);
            }
        }
        else
        {
            free(file_message_data->filename);
            free(file_message_data);
            gtk_widget_destroy(dialog);
        }
    }
    return 0;
}

//文件接收函数
void RecdServerFileMsg(const gchar *rcvd_text, uint16_t len, u_int32_t recd_uid)
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
        file_message_recv(rcvd_text, userinfo, len);
    }
}

//图片处理函数
int deal_with_recv_message(void *data)
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

//接收图片函数
int image_message_recv(const gchar *recv_text, FriendInfo *info, int charlen)
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
                    char strdest[17] = {0};
                    i += 2;
                    image_message_data->imagecount++;
                    memcpy(strdest, &recv_text[i], 16);
                    FindImage(strdest, image_message_data, deal_with_recv_message); //请求图片
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
    if ((event->button == 1) && (MarkUpdateInfo == 0))
    {
        FriendInfo *friendinforear;
        friendinforear = FriendInfoHead;
        while (friendinforear)
        {
            if ((friendinforear->user.uid == CurrentUserInfo->uid) && friendinforear->Infowind == NULL)
            {
                //查看资料
                OnlyLookInfo(friendinforear);
                MarkUpdateInfo = 1;
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

//设置按钮开始
//鼠标点击事件
static gint setup_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    if (event->button == 1)
    {              //设置按钮
        gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));  //设置鼠标光标
    }
    return 0;
}

//鼠标抬起事件
static gint setup_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if ((event->button == 1) && MarkNewpasswd == 0)       // 判断是否是点击设置按钮
    {
        SetupFace();
        MarkNewpasswd = 1;
    }
    return 0;
}

//鼠标移动事件
static gint setup_enter_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    return 0;
}

//离开事件
static gint setup_leave_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    return 0;
}
//设置按钮结束

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
                gtk_window_present(GTK_WINDOW(friendinforear->chartwindow));
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
                gtk_window_present(GTK_WINDOW(friendinforear->Infowind));
            }
        }
    }
    return 0;
}

//static gint search_button_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
//{
//    GtkWidget *add_surface=data;
//    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
//    gtk_image_set_from_surface((GtkImage *) add_surface, surface_status2);
//    return 0;
//}

//搜索放上去
static gint search_button_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *search2 = ChangeThem_png("搜索2.png");
    gtk_image_set_from_surface((GtkImage *) search, search2);
    return 0;
}

//搜索移走
static gint search_button_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *search1 = ChangeThem_png("搜索.png");
    gtk_image_set_from_surface((GtkImage *) search, search1);
    return 0;
}

static gint search_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *search1 = ChangeThem_png("搜索.png");
    gtk_image_set_from_surface((GtkImage *) search, search1);
    if (AddFriendflag)//判断是否打开搜索窗口
    {
        AddFriendFun();
        //Friend_Fequest_Popup(10001,"as");//添加爱弹出框

    } //调用添加好友函数
    return 0;
}


static gint change_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                        gpointer data)
{
    if (event->button == 1)       // 判断是否是点击关闭图标
    {
        gtk_image_set_from_surface((GtkImage *) change, surfacechangetheme2); //置换图标
        changethemeface();
    }
    return 0;
}


void set_position(GtkMenu *menu, gint *px, gint *py, gboolean *push_in, gpointer data)
{

    gdk_window_get_origin(GDK_WINDOW(window), px, py);
    *py += 10;

}

//放上去
static gint status_button_notify_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) status, surface_status2);
    return 0;
}

//按下
int status_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        gtk_image_set_from_surface((GtkImage *) status, surface_status2);
    }
    return 0;
}

//松开
int status_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));
    GtkMenu *menu_status = g_object_get_data(G_OBJECT(status), "ChangeMenu");
    //GtkCheckMenuItem *menu_status = g_object_get_data(G_OBJECT(status), "ChangeMenu");
    gtk_menu_popup(GTK_MENU(menu_status), NULL, NULL, set_position, NULL, event->button, event->time);

    gtk_image_set_from_surface((GtkImage *) status, surface_status);
    return 0;
}

//离开
static gint status_button_leave_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(window), gdk_cursor_new(GDK_ARROW));

    gtk_image_set_from_surface((GtkImage *) status, surface_status);
    return 0;
}

int MainInterFace()
{
    //一个关闭语音按钮的标志位。为１时表示语音已经打开，为０表示没有人在语音。
    flag_audio_close = 0;

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;//列表
    vbox = gtk_box_new(TRUE, 5);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);//固定窗口大小
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

    change_event_box = BuildEventBox(change,
                                     G_CALLBACK(change_button_release_event),
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL);

    search = gtk_image_new_from_surface(surfaceresearch);

    search_event_box = BuildEventBox(search,
                                     NULL,
                                     G_CALLBACK(search_button_notify_event),
                                     G_CALLBACK(search_button_leave_event),
                                     G_CALLBACK(search_button_release_event),
                                     NULL,
                                     NULL);


    setup_event_box = BuildEventBox(SetUp,
                                    G_CALLBACK(setup_button_press_event),
                                    G_CALLBACK(setup_enter_notify_event),
                                    G_CALLBACK(setup_leave_notify_event),
                                    G_CALLBACK(setup_button_release_event),
                                    NULL,
                                    NULL);


    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(background_event_box), 0, 0);//起始坐标
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(change_event_box), 240, 185);
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(closebut_event_box), 247, 0);
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(search_event_box), 0, 140);
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(setup_event_box), 205, 188);//设置按钮
    gtk_fixed_put(GTK_FIXED(MainLayout), friend, 1, 178);
    loadinfo();

    headx_event_box = BuildEventBox(headx,
                                    G_CALLBACK(headx_button_press_event),
                                    G_CALLBACK(headx_enter_notify_event),
                                    G_CALLBACK(headx_leave_notify_event),
                                    G_CALLBACK(headx_button_release_event),
                                    NULL,
                                    NULL);
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(headx_event_box), 10, 15);

    GtkWidget *online, *hideline;

    surface_status = ChangeThem_png("状态.png");
    surface_status2 = ChangeThem_png("状态2.png");
    status = gtk_image_new_from_surface(surface_status);

    GtkEventBox *status_event_box;
    status_event_box = BuildEventBox(status,
                                     G_CALLBACK(status_button_press_event),
                                     G_CALLBACK(status_button_notify_event),
                                     G_CALLBACK(status_button_leave_event),
                                     G_CALLBACK(status_button_release_event),
                                     NULL,
                                     NULL);
    gtk_fixed_put(GTK_FIXED(MainLayout), GTK_WIDGET(status_event_box), 220, 3);//起始坐标


    GtkWidget *changeMenu;

    changeMenu = gtk_menu_new();
    //在线
    online = gtk_check_menu_item_new();
    online = gtk_check_menu_item_new_with_mnemonic("在线");
    gtk_container_add(GTK_CONTAINER(changeMenu), online);
    gtk_widget_show(online);
    //隐身
    hideline = gtk_check_menu_item_new();
    hideline = gtk_check_menu_item_new_with_mnemonic("隐身");
    gtk_container_add(GTK_CONTAINER(changeMenu), hideline);
    gtk_widget_show(hideline);

    //添加在线事件
    g_signal_connect(G_OBJECT(online), "button_release_event",
                     G_CALLBACK(ChangeOnLine), (gpointer) changeMenu);
    //添加隐身事件
    g_signal_connect(G_OBJECT(hideline), "button_release_event",
                     G_CALLBACK(ChangeHideLine), (gpointer) changeMenu);

    g_object_set_data(G_OBJECT(status), "ChangeMenu", changeMenu);

    g_object_set_data(G_OBJECT(status), "OnlineMenu", online);
    g_object_set_data(G_OBJECT(status), "HidelineMenu", hideline);

    Status((void *) -1);//设置在线
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
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);
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
    GtkWidget *up;
    GtkWidget *down;
    GtkWidget *add;
    GtkWidget *delete;
    GtkWidget *rename;
    GtkWidget *addpeople;
    GtkWidget *Refresh;
    GtkWidget *sendmsg;
    GtkWidget *deletefriend;
    //GtkWidget *remark;
    GtkWidget *sendfile;
    GtkWidget *lookinfo;
    //分组菜单
    menu1 = gtk_menu_new();

    up = gtk_menu_item_new_with_mnemonic("分组上移");
    gtk_container_add(GTK_CONTAINER(menu1), up);
    gtk_widget_show(up);

    down = gtk_menu_item_new_with_mnemonic("分组下移");
    gtk_container_add(GTK_CONTAINER(menu1), down);
    gtk_widget_show(down);

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

    /*g_signal_connect(G_OBJECT(treeView), "button_press_event",
                     G_CALLBACK(button2_press_event2), (gpointer) menu1);*/
    //分组上移事件
    g_signal_connect(G_OBJECT(up), "button_release_event",
                     G_CALLBACK(UpGroupButtonPressEvent), treeView);

    //分组下移事件
    g_signal_connect(G_OBJECT(down), "button_press_event",
                     G_CALLBACK(DownGroupButtonPressEvent), treeView);

    //添加分组事件
    g_signal_connect(G_OBJECT(add), "button_release_event",
                     G_CALLBACK(AddGroupButtonPressEvent), (gpointer) menu1);

    //删除分组事件
    g_signal_connect(G_OBJECT(delete), "button_press_event",
                     G_CALLBACK(DeleteGroupButtonPressEvent), treeView);
    //重命名分组事件
    g_signal_connect(G_OBJECT(rename), "button_press_event",
                     G_CALLBACK(RenameGroupButtonPressEvent), treeView);
    //添加好友按钮

    g_signal_connect(G_OBJECT(addpeople), "button_release_event",
                     G_CALLBACK(search_button_release_event), treeView);
//    //添加好友鼠标放上去
//    g_signal_connect(G_OBJECT(addpeople), "button_press_event",
//                     G_CALLBACK(search_button_notify_event), addpeople);

    //好友菜单
    menu2 = gtk_menu_new();
    sendmsg = gtk_menu_item_new_with_mnemonic("发送即时消息");
    gtk_container_add(GTK_CONTAINER(menu2), sendmsg);
    gtk_widget_show(sendmsg);
    deletefriend = gtk_menu_item_new_with_mnemonic("删除好友");
    gtk_container_add(GTK_CONTAINER(menu2), deletefriend);
    gtk_widget_show(deletefriend);
    //remark = gtk_menu_item_new_with_mnemonic("修改备注");
    //gtk_container_add(GTK_CONTAINER(menu2), remark);
    //gtk_widget_show(remark);
    sendfile = gtk_menu_item_new_with_mnemonic("发送文件");
    gtk_container_add(GTK_CONTAINER(menu2), sendfile);
    gtk_widget_show(sendfile);
    lookinfo = gtk_menu_item_new_with_mnemonic("查看资料");
    gtk_container_add(GTK_CONTAINER(menu2), lookinfo);
    gtk_widget_show(lookinfo);
    friend_mov_group = gtk_menu_item_new_with_mnemonic("移动分组");
    gtk_container_add(GTK_CONTAINER(menu2), friend_mov_group);
    gtk_widget_show(friend_mov_group);

    g_signal_connect(G_OBJECT(deletefriend), "button_release_event",
                     G_CALLBACK(Friend_Delete_Popup), (gpointer) treeView);

    g_object_set_data(G_OBJECT(treeView), "GroupMenu", menu1);
    g_object_set_data(G_OBJECT(treeView), "FriendMenu", menu2);

    g_signal_connect(G_OBJECT(treeView), "button_release_event",
                     G_CALLBACK(button2_release_event), NULL);
    g_signal_connect(G_OBJECT(treeView), "button_press_event",
                     G_CALLBACK(button2_dblclick_event), NULL);

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

void DestoryMainInterface()
{
    gtk_widget_destroy(window);
}

int ShowStatus(void *data)
{
    //在线
    gtk_label_set_text(GTK_LABEL(StatusShowText), data);
    return 0;
}

int Status(void *data)//shezhi da goude
{
    GtkCheckMenuItem *online = g_object_get_data(G_OBJECT(status), "OnlineMenu");
    GtkCheckMenuItem *hideline = g_object_get_data(G_OBJECT(status), "HidelineMenu");

    if (data)
    {
        gtk_check_menu_item_set_active(online, 1);
        gtk_check_menu_item_set_active(hideline, FALSE);
    }
    else
    {
        gtk_check_menu_item_set_active(online, FALSE);
        gtk_check_menu_item_set_active(hideline, 1);
    }

    return 0;
}
