#include "Managegroup.h"
#include"ClientSockfd.h"
#include "MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include <string.h>
#include "PopupWinds.h"

GtkWidget *add_group_window;
GtkTextView *new_group_name;
//pop……………………………………………………………………………………………………

int pop(void *data)
{
    popup("系统消息", data);
    return 0;
}

//拖拽窗口
static gint add_group_mov(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    gdk_window_set_cursor(gtk_widget_get_window(add_group_window), gdk_cursor_new(GDK_ARROW));
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
    gtk_widget_destroy(add_group_window);
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
        // friends
        g_object_unref(pixbuf);

    }
    else
    {
        log_info("添加失败", "添加分组失败\n");

    }
    return 0;

}

//完成
static gint add_group_done(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    char *groupname = gtk_entry_get_text(new_group_name);//拿到分组名称



    UserGroup *group = calloc(1, sizeof(UserGroup));
    for (int i = 2; i < 256; i++)
    {

        if (UserFriendsGroupGet(friends, i) == NULL)
        {
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


int AddGroupButtonPressEvent()
{
    log_info("ADD", "\n");
    GtkWidget *add_group_framelayout, *add_group_layout;
    GtkTextView *title;
    GtkEventBox *addgroup_mov_event, *addgroup_done_event, *addgroup_close_event;

    GtkWidget *background, *done, *close;
    cairo_surface_t *surface_back, *surfacedone, *surfaceclose;
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
    surfaceclose = cairo_image_surface_create_from_png("关闭按钮1.png");

    done = gtk_image_new_from_surface(surfacedone);
    background = gtk_image_new_from_surface(surface_back);
    close = gtk_image_new_from_surface(surfaceclose);


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
    //关闭事件
    addgroup_close_event = BuildEventBox(
            close,
            G_CALLBACK(close_button_release_event),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);

    gtk_fixed_put(GTK_FIXED(add_group_layout), addgroup_mov_event, 0, 0);//窗口移动事件
    gtk_fixed_put(GTK_FIXED(add_group_layout), title, 65, 90);          //标题
    gtk_fixed_put(GTK_FIXED(add_group_layout), new_group_name, 45, 130);//新分组名称输入框
    gtk_fixed_put(GTK_FIXED(add_group_layout), addgroup_done_event, 65, 160);//完成事件按钮
    gtk_fixed_put(GTK_FIXED(add_group_layout), addgroup_close_event, 210, 0);//完成事件按钮


    gtk_container_add(GTK_CONTAINER(add_group_framelayout), add_group_layout);
    gtk_container_add(GTK_CONTAINER(add_group_window), add_group_framelayout);
    gtk_widget_show_all(add_group_window);

    return 0;

}


//删除分组………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

uint32_t del_usg;
GtkTreeIter del_iterGroup;

int delete_group_recv(CRPBaseHeader *header, void *data)
{
    if (header->packetID = CRP_PACKET_OK)
    {
        gtk_tree_store_remove(TreeViewListStore, &del_iterGroup);
        UserFriendsGroupDelete(friends, del_usg);
        return 0;
    }
    else
    {
        g_idle_add(pop, "删除失败");
    }
}


int DeleteGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data)
{

    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &del_iterGroup);

    if (gtk_tree_model_iter_has_child(model, &del_iterGroup) == 0)//没有好友
    {


        gtk_tree_model_get(model, &del_iterGroup, FRIENDUID_COL, &del_usg, -1);//分组id
        log_info("Group id", "%u\n", del_usg);


        if (del_usg == 0 || del_usg == 1)
        {
            popup("系统消息", "不能删除默认分组");
        }
        else
        {
            session_id_t sessionid = CountSessionId();//注册会话接受服务器
            AddMessageNode(sessionid, delete_group_recv, NULL);
            CRPFriendGroupDeleteSend(sockfd, sessionid, del_usg);//删除分组请求

        }

    }
    else
    {
        popup("系统消息", "不能删除非空分组");
    }


    log_info("删除分组", "\n");
    return 0;
}

//重命名分组…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
uint8_t rename_group_id;
GtkWidget *new_group_text;

int rename_group(void *data)
{
    UserGroup *fakeGroup = data;

    UserGroup *realGroup = UserFriendsGroupGet(friends, rename_group_id);//更新friends对象
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
    if (header->packetID = CRP_PACKET_OK)
    {

        g_idle_add(rename_group, data);

    }
    else
    {
        g_idle_add(pop, "重命名失败");

    }
    return 0;
}

//确定按钮
static gint done_rename_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    char *rename_group_name;//分组名称
    rename_group_name = gtk_entry_get_text(new_group_text);

    if (rename_group_id == 1 || rename_group_id == 0)//黑名单或者我的好友
    {
        popup("系统消息", "不能修改默认分组");
    }
    else
    {

        UserGroup *group = calloc(1, sizeof(UserGroup));

        group->groupId = rename_group_id;
        memcpy(group->groupName, rename_group_name, sizeof(rename_group_name));

        session_id_t sessionid = CountSessionId();//注册会话接受服务器
        AddMessageNode(sessionid, rename_group_recv, group);
        CRPFriendGroupRenameSend(sockfd,
                                 sessionid,
                                 rename_group_id,
                                 rename_group_name);
    }

    gtk_widget_destroy(GTK_WIDGET(data));
    return 0;
}

//关闭按钮
static gint close_rename_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return 0;
}


int RenameGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkWidget *renamewindow, *renameframelayout, *renamelayout;

    renamewindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    renameframelayout = gtk_layout_new(NULL, NULL);
    renamelayout = gtk_fixed_new();


    gtk_window_set_position(GTK_WINDOW(renamewindow), GTK_WIN_POS_CENTER);//窗口出现位置
    gtk_window_set_resizable(GTK_WINDOW (renamewindow), FALSE);//窗口不可改变
    gtk_window_set_decorated(GTK_WINDOW(renamewindow), FALSE);   // 去掉边框
    gtk_widget_set_size_request(GTK_WIDGET(renamewindow), 220, 70);//窗口大小

//关闭事件
    cairo_surface_t *rename_surface_close;
    GtkWidget *rename_close;
    GtkEventBox *rename_close_event_box;
    rename_surface_close = cairo_image_surface_create_from_png("关闭按钮1.png");
    rename_close = gtk_image_new_from_surface(rename_surface_close);
    rename_close_event_box = BuildEventBox(
            rename_close,
            G_CALLBACK(close_rename_event),
            NULL,
            NULL,
            NULL,
            NULL,
            renamewindow);
    gtk_fixed_put(GTK_FIXED(renamelayout), rename_close_event_box, 170, 0);

//新组名,组id
    new_group_text = gtk_entry_new();//新组名输入
    gtk_fixed_put(GTK_FIXED(renamelayout), new_group_text, 0, 0);

    rename_group_id = get_group_id(data);


//确定事件，发送请求
    cairo_surface_t *rename_surface_done;
    GtkWidget *rename_done;
    GtkEventBox *rename_done_event_box;
    rename_surface_done = cairo_image_surface_create_from_png("确定.png");
    rename_done = gtk_image_new_from_surface(rename_surface_done);
    rename_done_event_box = BuildEventBox(
            rename_done,
            G_CALLBACK(done_rename_event),
            NULL,
            NULL,
            NULL,
            NULL,
            renamewindow);
    gtk_fixed_put(GTK_FIXED(renamelayout), rename_done_event_box, 170, 30);


    gtk_container_add(GTK_CONTAINER (renamewindow), renameframelayout);
    gtk_container_add(GTK_CONTAINER (renameframelayout), renamelayout);
    gtk_widget_show_all(renamewindow);
    return 0;
}

//分组上移……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

int up_group(void *data)
{

//遍历iter
    uint32_t gid = data;
    uint32_t usg;
    int64_t priority;
    int64_t next_priority;
    GtkTreeIter up_itergroup;
    log_info("选中的之前组ID2", "%u\n", gid);


    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup, FRIENDUID_COL, &usg, -1);//第一个分组id

    while (1)
    {
        if (usg == gid)//第一个就是要改变的
        {
            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                               &up_itergroup,
                               PRIORITY_COL,
                               &priority,
                               -1);//拿到第1个PRIORITY_COL
            gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);//后移
            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                               &up_itergroup,
                               PRIORITY_COL,
                               &next_priority,
                               -1);//拿到第2个PRIORITY_COL
            break;
        }

        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup))//没有找到的华结束循环
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup, FRIENDUID_COL, &usg, -1);//分组id

    }

    log_info("PRIORITY_COL", "%d\n", priority);
    log_info("PRIORITY_next COL", "%d\n", next_priority);


    gtk_tree_store_set(TreeViewListStore, &up_itergroup,
                       PRIORITY_COL, priority,
                       -1);
    gtk_tree_model_iter_previous(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);//前移

    gtk_tree_store_set(TreeViewListStore, &up_itergroup,
                       PRIORITY_COL, next_priority,
                       -1);
    return 0;
}

int up_group_recv(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_OK)
    {
        g_idle_add(up_group, data);
    }
    else
    {
        log_info("上移失败\n", "");
    }
}


int UpGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int64_t up_priority;
    int64_t previous_priority;
    uint32_t up_gid;
    uint32_t pre_gid;
    GtkTreeIter up_iter_group;
    GtkTextIter tem_iter;


    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &up_iter_group);//拿到选中的分组

    gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &up_gid, -1);//拿到选中的组id
    gtk_tree_model_get(model, &up_iter_group, PRIORITY_COL, &up_priority, -1);//拿到选中的权值



    // memcpy(&tem_iter, &up_iter_group, sizeof(up_iter_group));
    if (gtk_tree_model_iter_previous(GTK_TREE_MODEL(TreeViewListStore), &up_iter_group) != 0)
    {
        gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &pre_gid, -1);//拿到选中的前一个组id


        log_info("选中的之前组ID1", "%u\n", pre_gid);


        session_id_t sessionid = CountSessionId();
        AddMessageNode(sessionid, up_group_recv, pre_gid);
        CRPFriendGroupMoveSend(sockfd, sessionid, up_gid, pre_gid);


//    log_info("选中的上一个权值", "%d\n",up_iter_group);
    }
    else
    {
        g_idle_add(pop, "已经是第一个分组");
    }

    return 0;
}


//分组下移……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………


int down_grou(void *data)
{
    uint32_t gid = data;
    uint32_t usg;
    int64_t priority;
    int64_t next_priority;
    GtkTreeIter up_itergroup;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup, FRIENDUID_COL, &usg, -1);//第一个分组id

    while (1)
    {
        if (usg == gid)//第一个就是要改变的
        {
            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                               &up_itergroup,
                               PRIORITY_COL,
                               &priority,
                               -1);//拿到第1个PRIORITY_COL
            gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);//后移
            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore),
                               &up_itergroup,
                               PRIORITY_COL,
                               &next_priority,
                               -1);//拿到第2个PRIORITY_COL
            break;
        }

        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup))//没有找到的华结束循环
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup, FRIENDUID_COL, &usg, -1);//分组id

    }

    log_info("PRIORITY_COL", "%d\n", priority);
    log_info("PRIORITY_next COL", "%d\n", next_priority);


    gtk_tree_store_set(TreeViewListStore, &up_itergroup,
                       PRIORITY_COL, priority,
                       -1);
    gtk_tree_model_iter_previous(GTK_TREE_MODEL(TreeViewListStore), &up_itergroup);//前移

    gtk_tree_store_set(TreeViewListStore, &up_itergroup,
                       PRIORITY_COL, next_priority,
                       -1);
    return 0;
}

int down_group_recv(CRPBaseHeader *header, void *data)
{

    if (header->packetID == CRP_PACKET_OK)
    {
//遍历iter
        g_idle_add(down_grou, data);

    }
    else
    {
    }
}

int DownGroupButtonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int64_t up_priority;
    int64_t next_priority;

    uint32_t current_gid;
    uint32_t next_gid;
    GtkTreeIter up_iter_group;
    GtkTextIter tem_iter;


    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &up_iter_group);//拿到选中的分组

    gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &current_gid, -1);//拿到选中的组id
    gtk_tree_model_get(model, &up_iter_group, PRIORITY_COL, &up_priority, -1);//拿到选中的权值




    //memcpy(&tem_iter, &up_iter_group, sizeof(up_iter_group));
    if (gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_iter_group) != 0)
    {
        gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &next_gid, -1);//拿到选中的后一个权值



        session_id_t sessionid = CountSessionId();
        AddMessageNode(sessionid, down_group_recv, current_gid);
        CRPFriendGroupMoveSend(sockfd, sessionid, next_gid, current_gid);

    }
    else
    {
        g_idle_add(pop, "已经是最后一个分组");

    }

    return 0;
}




//拿到分组ID的函数……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

int get_group_id(gpointer data)
{
    GtkTreeIter iterGroup;
    uint8_t usg;

    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iterGroup);

    gtk_tree_model_get(model, &iterGroup, FRIENDUID_COL, &usg, -1);//第一个分组id
    log_info("Group id", "%u\n", usg);


    log_info("删除分组", "\n");
    return usg;

}