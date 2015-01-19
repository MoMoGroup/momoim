#include"../ClientSockfd.h"
#include "../MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include <string.h>
#include"ManageGroup.h"

typedef struct addGroupStruct
{
    GtkWidget *add_group_window;
    GtkWidget *new_group_name;
    GtkWidget *done;
    GtkWidget *close;

} addGroupStruct;

//拖拽窗口
static gint add_group_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gdk_window_set_cursor(gtk_widget_get_window(window_widget->add_group_window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1)
    { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   (gint) event->x_root, (gint) event->y_root, event->time);
    }
    return 0;
}

//关闭按钮
static gint close_add_group_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gdk_window_set_cursor(gtk_widget_get_window(window_widget->add_group_window), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_close2 = ChangeThem_png("关闭按钮2.png");
    gtk_image_set_from_surface((GtkImage *) window_widget->close, surface_close2);
    return 0;
}

static gint close_add_group_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gdk_window_set_cursor(gtk_widget_get_window(window_widget->add_group_window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_close1 = ChangeThem_png("关闭按钮1.png");
    gtk_image_set_from_surface((GtkImage *) window_widget->close, surface_close1);
    return 0;
}

static gint add_close_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gtk_widget_destroy(window_widget->add_group_window);
    free(data);//关闭了，free结构题内存
    return 0;
}

//添加分组的请求发出后，接收服务器的函数
int add_group_recv(CRPBaseHeader *header, void *data)
{
    UserGroup *group = data;
    if (header->packetID == CRP_PACKET_OK)//成功
    {

        GtkTreeIter itergroup;
        cairo_surface_t *surface;
        GdkPixbuf *pixbuf;
        cairo_t *cr;
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 260, 33);
        cr = cairo_create(surface);
        cairo_move_to(cr, 0, 20);
        cairo_set_font_size(cr, 14);
        cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_show_text(cr, group->groupName);//分组名称
        pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, 260, 33);

        gtk_tree_store_append(TreeViewListStore, &itergroup, NULL);
        gtk_tree_store_set(TreeViewListStore, &itergroup,
                           PIXBUF_COL, pixbuf,
                           FRIENDUID_COL, (uint32_t) group->groupId,
                           PRIORITY_COL, (int64_t) -group->groupId,
                           -1);
        UserFriendsGroupAdd(friends, group->groupId, group->groupName);
        g_idle_add(GroupPop, "添加分组成功");
        // friends
        g_object_unref(pixbuf);

    }
    else
    {
        log_info("添加失败", "添加分组失败\n");

    }
    return 0;

}

//确定按钮放上去
static gint add_group_notify_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gdk_window_set_cursor(gtk_widget_get_window(window_widget->add_group_window), gdk_cursor_new(GDK_HAND2));
    cairo_surface_t *surface_next2 = ChangeThem_png("确定2.png");
    gtk_image_set_from_surface((GtkImage *) window_widget->done, surface_next2);
    return 0;
}

//移走
static gint add_group_leave_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    gdk_window_set_cursor(gtk_widget_get_window(window_widget->add_group_window), gdk_cursor_new(GDK_ARROW));
    cairo_surface_t *surface_next1 = ChangeThem_png("确定.png");
    gtk_image_set_from_surface((GtkImage *) window_widget->done, surface_next1);
    return 0;
}

//完成按钮按下
static gint add_group_done(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    addGroupStruct *window_widget = data;
    const char *groupname = gtk_entry_get_text((GtkEntry *) window_widget->new_group_name);//拿到分组名称


    UserGroup *group = calloc(1, sizeof(UserGroup));
    //循环找到一个空分组id
    for (uint8_t i = 2; i < 256; i++)
    {

        if (UserFriendsGroupGet(friends, i) == NULL)
        {
            group->groupId = i;
            memcpy(group->groupName, groupname, strlen(groupname));
            break;
        }
    }
    session_id_t sessionid = CountSessionId();//注册会话接受服务器
    AddMessageNode(sessionid, add_group_recv, group);
    CRPFriendGroupAddSend(sockfd, sessionid, group->groupId, groupname);//发送添加分组请求

    gtk_widget_destroy(window_widget->add_group_window);
    free(data);//释放结构体内存
    return 0;
}


int AddGroupButtonPressEvent()
{
//    GtkWidget *add_group_window;
//    GtkWidget *new_group_name;
    addGroupStruct *window_widget = malloc(sizeof(struct addGroupStruct));


    GtkWidget *add_group_framelayout, *add_group_layout;
    GtkWidget *title;
    GtkEventBox *addgroup_mov_event, *addgroup_done_event, *addgroup_close_event;

//    GtkWidget *background, *done, *close;
    GtkWidget *background;
    cairo_surface_t *surface_back, *surfacedone, *surfaceclose;
    window_widget->add_group_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//初始化窗口
    add_group_framelayout = gtk_layout_new(NULL, NULL);//初始化窗口
    add_group_layout = gtk_fixed_new();//初始化窗口

    gtk_window_set_position(GTK_WINDOW(window_widget->add_group_window), GTK_WIN_POS_MOUSE);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (window_widget->add_group_window), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(window_widget->add_group_window), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(window_widget->add_group_window), 250, 235);
//资源
    surface_back = ChangeThem_png("提示框.png");
    surfacedone = ChangeThem_png("确定.png");
    surfaceclose = ChangeThem_png("关闭按钮1.png");

    window_widget->done = gtk_image_new_from_surface(surfacedone);
    background = gtk_image_new_from_surface(surface_back);
    window_widget->close = gtk_image_new_from_surface(surfaceclose);


    window_widget->new_group_name = gtk_entry_new();
    title = gtk_label_new("输入分组名称");


    // 设置窗体获取鼠标事件
    addgroup_mov_event = BuildEventBox(
            background,
            G_CALLBACK(add_group_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            window_widget);

    //完成按钮事件
    addgroup_done_event = BuildEventBox(
            window_widget->done,
            NULL,
            G_CALLBACK(add_group_notify_event),
            G_CALLBACK(add_group_leave_event),
            G_CALLBACK(add_group_done),
            NULL,
            window_widget);

    //关闭事件
    addgroup_close_event = BuildEventBox(
            window_widget->close,
            NULL,
            G_CALLBACK(close_add_group_notify_event),
            G_CALLBACK(close_add_group_leave_event),
            G_CALLBACK(add_close_button_release_event),
            NULL,
            window_widget);

    gtk_fixed_put(GTK_FIXED(add_group_layout), GTK_WIDGET(addgroup_mov_event), 0, 0);//窗口移动事件
    gtk_fixed_put(GTK_FIXED(add_group_layout), GTK_WIDGET(title), 65, 90);          //标题
    gtk_fixed_put(GTK_FIXED(add_group_layout), GTK_WIDGET(window_widget->new_group_name), 45, 130);//新分组名称输入框
    gtk_fixed_put(GTK_FIXED(add_group_layout), GTK_WIDGET(addgroup_done_event), 75, 170);//完成事件按钮
    gtk_fixed_put(GTK_FIXED(add_group_layout), GTK_WIDGET(addgroup_close_event), 210, 0);//完成事件按钮


    gtk_container_add(GTK_CONTAINER(add_group_framelayout), add_group_layout);
    gtk_container_add(GTK_CONTAINER(window_widget->add_group_window), add_group_framelayout);
    gtk_widget_show_all(window_widget->add_group_window);

    return 0;

}