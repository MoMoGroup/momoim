#include"ClientSockfd.h"
#include "MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include <string.h>
#include"ManageGroup.h"
#include "PopupWinds.h"

//重命名分组…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………


typedef struct renameGroupStruct
{
    uint8_t rename_group_id;
    GtkWidget *new_group_text;
    GtkWidget *window;
    GtkWidget *close;
    GtkWidget *done;
} renameGroupStruct;


int rename_group(void *data)
{
    renameGroupStruct *win = data;

    UserGroup *fakeGroup = data;

    UserGroup *realGroup = UserFriendsGroupGet(friends, win->rename_group_id);//更新friends对象
    memcpy(realGroup->groupName, fakeGroup->groupName, sizeof(realGroup->groupName));


    uint32_t rename_usg;
    GtkTreeIter rename_itergroup;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &rename_itergroup);
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                       &rename_itergroup,
                       FRIENDUID_COL,
                       &rename_usg,
                       -1);//第一个分组id

    while (1)
    {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &rename_itergroup))
        {
            break;
        }

        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                           &rename_itergroup,
                           FRIENDUID_COL,
                           &rename_usg,
                           -1);//拿到id
        if (rename_usg == fakeGroup->groupId)
        {
            //说明找到了，结束循环
            break;
        }
    }


    cairo_surface_t *surface;
    GdkPixbuf *pixbuf;
    cairo_t *cr;
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 33);
    cr = cairo_create(surface);
    cairo_move_to(cr, 0, 20);
    cairo_set_font_size(cr, 14);
    cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_show_text(cr, fakeGroup->groupName);//分组名称
    pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 33);

    gtk_tree_store_set(TreeViewListStore, &rename_itergroup,
                       PIXBUF_COL, pixbuf,
                       -1);
    g_object_unref(pixbuf);
    return 0;
}

int rename_group_recv(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_OK)
    {

        g_idle_add(rename_group, data);

    }
    else
    {
        g_idle_add(GroupPop, "重命名失败");

    }
    return 0;
}

//拖拽窗口
static gint rename_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(data), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

//确定按钮放上去
static gint done_rename_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    cairo_surface_t *surface_done2 = ChangeThem_png("确定2.png");

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(win->window)), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) win->done, surface_done2);
    return 0;
}

//确定按钮移走
static gint done_rename_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    cairo_surface_t *surface_done1 = ChangeThem_png("确定.png");

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(win->window)), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) win->done, surface_done1);
    return 0;
}

//确定按钮按下
static gint done_rename_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    gchar *rename_group_name;//分组名称
    rename_group_name = gtk_entry_get_text(GTK_ENTRY(win->new_group_text));

    if (win->rename_group_id == 1 || win->rename_group_id == 0)//黑名单或者我的好友
    {
        Popup("系统消息", "不能修改默认分组");
    }
    else
    {

        UserGroup *group = calloc(1, sizeof(UserGroup));

        group->groupId = win->rename_group_id;
        memcpy(group->groupName, rename_group_name, sizeof(rename_group_name));

        session_id_t sessionid = CountSessionId();//注册会话接受服务器
        AddMessageNode(sessionid, rename_group_recv, group);
        CRPFriendGroupRenameSend(sockfd,
                                 sessionid,
                                 win->rename_group_id,
                                 rename_group_name);
    }

    gtk_widget_destroy(GTK_WIDGET(win->window));
    free(data);
    return 0;
}


//关闭按钮放上去
static gint close_rename_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    cairo_surface_t *surface_close1 = ChangeThem_png("关闭按钮2.png");

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(win->window)), gdk_cursor_new(GDK_HAND2));
    gtk_image_set_from_surface((GtkImage *) win->close, surface_close1);
    return 0;
}

//关闭按钮移走
static gint close_rename_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    cairo_surface_t *surface_close2 = ChangeThem_png("关闭按钮1.png");

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(win->window)), gdk_cursor_new(GDK_ARROW));
    gtk_image_set_from_surface((GtkImage *) win->close, surface_close2);
    return 0;
}

//关闭按钮按下
static gint close_rename_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = data;
    gtk_widget_destroy(GTK_WIDGET(win->window));
    free(win);
    return 0;
}


int RenameGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    renameGroupStruct *win = malloc(sizeof(struct renameGroupStruct));

    GtkWidget *renamelayout;
    //GtkWidget *renamewindow;
    GtkWidget *renameframelayout;
//    GtkWidget *rename_done;
//    GtkWidget *rename_close;
    GtkWidget *rename_background;
    GtkWidget *title;

    cairo_surface_t *rename_surface_done;
    cairo_surface_t *surface_background;
    cairo_surface_t *rename_surface_close;

    //事件盒子
    GtkEventBox *rename_mov_event;
    GtkEventBox *rename_close_event_box;
    GtkEventBox *rename_done_event_box;

//layout
    renamelayout = gtk_fixed_new();
    renameframelayout = gtk_layout_new(NULL, NULL);
    win->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

//初始化窗口
    gtk_window_set_position(GTK_WINDOW(win->window), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (win->window), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(win->window), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(win->window), 250, 235);//窗口大小


    surface_background = ChangeThem_png("提示框.png");
    rename_surface_close = ChangeThem_png("关闭按钮1.png");
    rename_surface_done = ChangeThem_png("确定.png");

    rename_background = gtk_image_new_from_surface(surface_background);
    win->close = gtk_image_new_from_surface(rename_surface_close);
    win->done = gtk_image_new_from_surface(rename_surface_done);

    //新组名,组id
    win->new_group_text = gtk_entry_new();//新组名输入
    win->rename_group_id = (uint8_t) get_iter_id(data);

    if (win->rename_group_id == 1 || win->rename_group_id == 0)//黑名单或者我的好友
    {
        Popup("系统消息", "不能修改默认分组");
        return 0;
    }
    else
    {
        title = gtk_label_new("请输入分组新名称");

//背景事件
        rename_mov_event = BuildEventBox(
                rename_background,
                G_CALLBACK(rename_mov),
                NULL,
                NULL,
                NULL,
                NULL,
                win->window);

//关闭事件
        rename_close_event_box = BuildEventBox(
                win->close,
                NULL,
                G_CALLBACK(close_rename_notify_event),
                G_CALLBACK(close_rename_leave_event),
                G_CALLBACK(close_rename_release_event),
                NULL,
                win);

//确定事件，发送请求
        rename_done_event_box = BuildEventBox(
                win->done,
                NULL,
                G_CALLBACK(done_rename_notify_event),
                G_CALLBACK(done_rename_leave_event),
                G_CALLBACK(done_rename_event),
                NULL,
                win);

        //put
        gtk_fixed_put(GTK_FIXED(renamelayout), GTK_WIDGET(rename_mov_event), 0, 0);//窗口移动事件
        gtk_fixed_put(GTK_FIXED(renamelayout), GTK_WIDGET(rename_close_event_box), 210, 0);
        gtk_fixed_put(GTK_FIXED(renamelayout), GTK_WIDGET(rename_done_event_box), 75, 170);
        gtk_fixed_put(GTK_FIXED(renamelayout), win->new_group_text, 45, 130);
        gtk_fixed_put(GTK_FIXED(renamelayout), GTK_WIDGET(title), 65, 90);          //标题


        gtk_container_add(GTK_CONTAINER (win->window), renameframelayout);
        gtk_container_add(GTK_CONTAINER (renameframelayout), renamelayout);
        gtk_widget_show_all(win->window);
        return 0;
    }
}
//拿到分组ID的函数……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

int get_iter_id(gpointer data)
{
    GtkTreeIter iterGroup;
    uint32_t usg;

    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iterGroup);

    gtk_tree_model_get(model, &iterGroup, FRIENDUID_COL, &usg, -1);//第一个分组id
    log_info("Group id", "%u\n", usg);


    log_info("删除分组", "\n");
    return usg;

}