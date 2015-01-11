#include "Managegroup.h"
#include <gtk/gtk.h>
#include"ClientSockfd.h"
#include "MainInterface.h"
#include "PopupWinds.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include <string.h>
#include <imcommon/friends.h>

GtkWidget *add_group_window;
GtkTextView *new_group_name;

//拖拽窗口
static gint add_group_mov(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    gdk_window_set_cursor(gtk_widget_get_window(add_group_window), gdk_cursor_new(GDK_ARROW));
    if (event->button == 1) { //gtk_widget_get_toplevel 返回顶层窗口 就是window.
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                                   event->x_root, event->y_root, event->time);
    }
    return 0;
}

//添加分组的请求发出后，接收服务器的函数
int add_group_recv(CRPBaseHeader *header, void *data) {
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
                           PRIORITY_COL, (int64_t) 0,
                           -1);
        UserFriendsGroupAdd(friends, group->groupId, group->groupName);
        // friends
        g_object_unref(pixbuf);

    }
    else {
        log_info("添加失败", "添加分组失败\n");

    }
    return 0;

}

//完成
static gint add_group_done(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    char *groupname = gtk_entry_get_text(new_group_name);//拿到分组



    UserGroup *group = calloc(1, sizeof(UserGroup));
    for (int i = 2; i < 256; i++) {

        if (UserFriendsGroupGet(friends, i) == NULL) {
            group->groupId = i;
            memcpy(group->groupName, groupname, sizeof(groupname));
            break;
        }
    }
    session_id_t sessionid = CountSessionId();//注册会话接受服务器
    AddMessageNode(sessionid, add_group_recv, group);

    CRPFriendGroupAddSend(sockfd, sessionid, group->groupId, groupname);//发送添加分组请求

    gtk_widget_destroy(add_group_window);
    return 0;
}

int add_group_Interface_fun() {
    GtkWidget *add_group_framelayout, *add_group_layout;
    GtkTextView *title;
    GtkEventBox *addgroup_mov_event, *addgroup_done_event;

    GtkWidget *background, *done;
    cairo_surface_t *surface_back, *surfacedone;
    add_group_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//初始化窗口
    add_group_framelayout = gtk_layout_new(NULL, NULL);//初始化窗口
    add_group_layout = gtk_fixed_new();//初始化窗口

    gtk_window_set_position(GTK_WINDOW(add_group_window), GTK_WIN_POS_MOUSE);//窗口位置
    gtk_window_set_resizable(GTK_WINDOW (add_group_window), FALSE);//固定窗口大小
    gtk_window_set_decorated(GTK_WINDOW(add_group_window), FALSE);//去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(add_group_window), 250, 235);
//资源
    surface_back = cairo_image_surface_create_from_png("提示框.png");
    surfacedone = cairo_image_surface_create_from_png("确定.png");

    done = gtk_image_new_from_surface(surfacedone);
    background = gtk_image_new_from_surface(surface_back);


    new_group_name = gtk_entry_new();
    title = gtk_text_view_new();
    gtk_test_text_set(title, "输入分组名称");


    // 设置窗体获取鼠标事件
    addgroup_mov_event = BuildEventBox(
            background,
            G_CALLBACK(add_group_mov),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
    //完成事件
    addgroup_done_event = BuildEventBox(
            done,
            G_CALLBACK(add_group_done),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    gtk_fixed_put(GTK_FIXED(add_group_layout), addgroup_mov_event, 0, 0);//窗口移动事件
    gtk_fixed_put(GTK_FIXED(add_group_layout), new_group_name, 45, 130);//新分组名称输入框
    gtk_fixed_put(GTK_FIXED(add_group_layout), title, 65, 90);          //标题
    gtk_fixed_put(GTK_FIXED(add_group_layout), addgroup_done_event, 65, 160);//完成事件按钮


    gtk_container_add(GTK_CONTAINER(add_group_framelayout), add_group_layout);
    gtk_container_add(GTK_CONTAINER(add_group_window), add_group_framelayout);
    gtk_widget_show_all(add_group_window);

    return 0;

}

int Add_Group_Button_Press_Event() {
    add_group_Interface_fun();
    //CRPFriendGroupAddSend(sockfd, <#(uint32_t)sessionID#>, <#(uint8_t)gid#>, <#(char const[64])name#>);
    log_info("ADD", "\n");
    return 0;

}

