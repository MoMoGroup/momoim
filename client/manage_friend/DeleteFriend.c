#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <glib-unix.h>
#include <cairo-script-interpreter.h>
#include <protocol/friend/Delete.h>
#include <protocol/friend/Notify.h>
#include <UpdataFriendList.h>
#include <MainInterface.h>
#include "common.h"
#include "ManageGroup.h"

static GtkWidget *delete_window;
static uint32_t delete_uid, delete_gid;
static GtkWidget *delete_cancel, *delete_done, *delete_background;

//拖拽窗口
static gint delete_drag_event_fun(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

//如果服务区发来更新好友包，说明删除成功，
int RemoveFriend(void *data)
{
    CRPPacketFriendNotify *deleteInfo = (CRPPacketFriendNotify *) data;

    GtkTreeIter iterUser;
    iterUser = GetUserIter(deleteInfo->uid);//此函数返回参数uid对应的iter

    gtk_tree_store_remove(TreeViewListStore, &iterUser);

    return 0;
}


//如果服务区发来FAILURE包，说明删除好友失败，弹窗提醒
int delete_friend_recv_fun(CRPBaseHeader *header, void *data)
{

    if (header->packetID == CRP_PACKET_FAILURE)//删除成功
    {
        g_idle_add(GroupPop, "删除好友失败");
        return 0;
    }
    else
    {
        UserGroup *group = UserFriendsGroupGet(friends, (uint8_t) delete_gid);
        UserFriendsUserDelete(group, delete_uid);//更新资料
        g_idle_add(GroupPop, "删除好友成功");

    }
    return 0;
}

//忽略按钮放上去
static gint delete_cancel_notify_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_cancel2 = ChangeThem_png("资料取消2.png");
    gtk_image_set_from_surface((GtkImage *) delete_cancel, surface_cancel2);
    return 0;
}

//quxiao按钮移走
static gint delete_cancel_leave_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_cancel1 = ChangeThem_png("资料取消.png");
    gtk_image_set_from_surface((GtkImage *) delete_cancel, surface_cancel1);
    return 0;
}

//如果取消删除
int delete_cancle_event_fun()
{
    gtk_widget_destroy(delete_window);
    DelFriendFlag = 1;//可以打开了
    return 0;
}

//确定按钮放上去
static gint delete_button_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_done2 = ChangeThem_png("确定2.png");
    gtk_image_set_from_surface((GtkImage *) delete_done, surface_done2);
    return 0;
}

//确定按钮移走
static gint delete_button_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)         // 鼠标移动事件
{
    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_done1 = ChangeThem_png("确定.png");
    gtk_image_set_from_surface((GtkImage *) delete_done, surface_done1);
    return 0;
}

//确定删除
int delete_done_event_fun()
{
    session_id_t sessionid = CountSessionId();
    AddMessageNode(sessionid, delete_friend_recv_fun, NULL);
    CRPFriendDeleteSend(sockfd, sessionid, delete_uid, (uint8_t) delete_gid);//发送删除好友的请求
    gtk_widget_destroy(delete_window);
    DelFriendFlag = 1;//可以打开了
    return 0;

}

//首先弹出是否确定删除此好友
int Friend_Delete_Popup(GtkWidget *widget, GdkEventButton *event, GtkTreeView *treeview)
{
    if (DelFriendFlag)
    {
        DelFriendFlag = 0;
        GtkWidget *delete_framelayout, *delete_layout;

        cairo_surface_t *surface_delete_cancel, *surface_delete_done, *surface_delete_background;
        GtkEventBox *delete_cancel_eventbox, *delete_done_eventbox, *delete_drag_eventbox;
        char str[100];
        char delete_name[32];
        GtkTreeIter itergroup, iteruser;
        GtkWidget *txt, *xitong;
//首先拿到选中好友所在分组id,和好友uid
        GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
        GtkTreeModel *model = gtk_tree_view_get_model(treeview);
        gtk_tree_selection_get_selected(selection, &model, &iteruser);//拿到选中的列
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iteruser, FRIENDUID_COL, &delete_uid, -1);

        gtk_tree_model_iter_parent(GTK_TREE_MODEL(TreeViewListStore), &itergroup, &iteruser);
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &itergroup, FRIENDUID_COL, &delete_gid, -1);

        log_info("要删除好友Uid", "%d\n", delete_uid);
        log_info("要删除好友分组Gid", "%u\n", delete_gid);

        delete_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        delete_framelayout = gtk_layout_new(NULL, NULL);
        delete_layout = gtk_fixed_new();

        gtk_window_set_position(GTK_WINDOW(delete_window), GTK_WIN_POS_CENTER);//窗口位置
        gtk_window_set_resizable(GTK_WINDOW (delete_window), FALSE);//固定窗口大小
        gtk_window_set_decorated(GTK_WINDOW(delete_window), FALSE);//去掉边框
        gtk_widget_set_size_request(GTK_WIDGET(delete_window), 250, 235);
//资源
        surface_delete_cancel = ChangeThem_png("资料取消.png");
        surface_delete_done = ChangeThem_png("确定.png");
        surface_delete_background = ChangeThem_png("提示框.png");
//获得
        delete_background = gtk_image_new_from_surface(surface_delete_background);
        delete_cancel = gtk_image_new_from_surface(surface_delete_cancel);
        delete_done = gtk_image_new_from_surface(surface_delete_done);

//拖动窗口
        delete_drag_eventbox = BuildEventBox(
                delete_background,
                G_CALLBACK(delete_drag_event_fun),
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);
        //取消删除
        delete_cancel_eventbox = BuildEventBox(
                delete_cancel,
                NULL,
                G_CALLBACK(delete_cancel_notify_event),
                G_CALLBACK(delete_cancel_leave_event),
                G_CALLBACK(delete_cancle_event_fun),
                NULL,
                NULL);

        //确定删除
        delete_done_eventbox = BuildEventBox(
                delete_done,
                NULL,
                G_CALLBACK(delete_button_notify_event),
                G_CALLBACK(delete_button_leave_event),
                G_CALLBACK(delete_done_event_fun),
                NULL,
                NULL);
//

        FriendInfo *p = FriendInfoHead;
        while (p->next)//拿到昵称
        {
            p = p->next;
            if (delete_uid == p->user.uid)
            {
                memcpy(delete_name, p->user.nickName, sizeof(p->user.nickName));
                break;
            }
        }

        sprintf(str, "确定删除%u(%s)吗?", delete_uid, delete_name);
        txt = gtk_label_new(str);//确定删除提示框

        xitong = gtk_label_new("系统消息");

        gtk_fixed_put(GTK_FIXED(delete_layout), GTK_WIDGET(delete_drag_eventbox), 0, 0);
        gtk_fixed_put(GTK_FIXED(delete_layout), xitong, 14, 10);//标题
        gtk_fixed_put(GTK_FIXED(delete_layout), txt, 50, 90);
        gtk_fixed_put(GTK_FIXED(delete_layout), GTK_WIDGET(delete_cancel_eventbox), 30, 170);
        gtk_fixed_put(GTK_FIXED(delete_layout), GTK_WIDGET(delete_done_eventbox), 150, 170);


        gtk_container_add(GTK_CONTAINER(delete_framelayout), delete_layout);
        gtk_container_add(GTK_CONTAINER(delete_window), delete_framelayout);
        gtk_widget_show_all(delete_window);
    }
    else
    {
        gtk_window_present(GTK_WINDOW(delete_window));
    }

    return 0;

}
