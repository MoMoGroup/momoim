#include <logger.h>
#include <ftlist.h>
#include <protocol/info/Data.h>
#include <glib-unix.h>
#include <cairo-script-interpreter.h>
#include <protocol/friend/Delete.h>
#include <protocol/friend/Notify.h>
#include "common.h"
#include "../MainInterface.h"
#include "../managegroup/ManageGroup.h"

static GtkWidget *delete_window;
static uint32_t delete_uid, delete_gid;
static GtkWidget *delete_cancel, *delete_done, *delete_background;


//如果服务区发来更新好友包，说明删除成功，
int RemoveFriend(void *data)
{
    CRPPacketFriendNotify *deleteInfo = (CRPPacketFriendNotify *) data;

    GtkTreeIter iterGroup, iterUser;
    uint32_t gid, uid;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &iterGroup);
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &gid, -1);//第一个分组id
    while (gid != deleteInfo->fromGid)//先循环先找到组
    {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterGroup))//没有下一个分组，结束循环
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &gid, -1);//分组id

    }

    //找到了分组之后
    gtk_tree_model_iter_children(GTK_TREE_MODEL(TreeViewListStore), &iterUser, &iterGroup);

    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterUser, FRIENDUID_COL, &uid, -1);//第一个好友id
    while (uid != deleteInfo->uid)//先循环先找到组
    {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterUser))//没有下一个好友，结束循环
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterUser, FRIENDUID_COL, &uid, -1);//拿到好友id

    }

    gtk_tree_store_remove(TreeViewListStore, &iterUser);

    return 0;
}


//如果服务区发来FAILURE包，说明删除失败，弹窗提醒
int delete_friend_recv_fun(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_FAILURE)//删除成功
    {
        g_idle_add(GroupPop, "删除失败");
        return 0;
    }
}


//如果取消删除
int delete_cancle_event_fun()
{
    gtk_widget_destroy(delete_window);
    return 0;
}

//确定按钮放上去
static gint delete_button_notify_event(GtkWidget *widget, GdkEventButton *event,
                                       gpointer data)
{
    gdk_window_set_cursor(gtk_widget_get_window(delete_window), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_done2 = ChangeThem_png("确定2.png");
    gtk_image_set_from_surface((GtkImage *) delete_done, surface_done2);
    return 0;
}

//确定按钮移走
static gint delete_button_leave_event(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data)         // 鼠标移动事件
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
    CRPFriendDeleteSend(sockfd, sessionid, delete_uid, delete_gid);//发送删除好友的请求
    gtk_widget_destroy(delete_window);
    return 0;

}

//首先弹出是否确定删除此好友
int Friend_Delete_Popup(GtkWidget *widget, GdkEventButton *event, GtkTreeView *treeview)
{
    GtkWidget *delete_framelayout, *delete_layout;

    cairo_surface_t *surface_delete_cancel, *surface_delete_done, *surface_delete_background;
    GtkEventBox *delete_cancel_eventbox, *delete_done_eventbox;

    GtkTreeIter itergroup, iteruser;
    GtkWidget *show;
//    GtkTreeIter itergroup, iteruser;
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
    surface_delete_cancel = ChangeThem_png("忽略1.png");
    surface_delete_done = ChangeThem_png("确定.png");
    surface_delete_background = ChangeThem_png("提示框.png");
//获得
    delete_background = gtk_image_new_from_surface(surface_delete_background);
    delete_cancel = gtk_image_new_from_surface(surface_delete_cancel);
    delete_done = gtk_image_new_from_surface(surface_delete_done);
    //取消删除
    delete_cancel_eventbox = BuildEventBox(
            delete_cancel,
            NULL,
            NULL,
            NULL,
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
    GtkWidget *txt;
    char str[80];
    sprintf(str, "确定删除%u吗", delete_uid);
    txt = gtk_label_new(str);


    gtk_fixed_put(GTK_FIXED(delete_layout), delete_background, 0, 0);
    gtk_fixed_put(GTK_FIXED(delete_layout), txt, 70, 90);
    gtk_fixed_put(GTK_FIXED(delete_layout), GTK_WIDGET(delete_cancel_eventbox), 30, 170);
    gtk_fixed_put(GTK_FIXED(delete_layout), GTK_WIDGET(delete_done_eventbox), 150, 170);


    gtk_container_add(GTK_CONTAINER(delete_framelayout), delete_layout);
    gtk_container_add(GTK_CONTAINER(delete_window), delete_framelayout);
    gtk_widget_show_all(delete_window);


    return 0;

}
