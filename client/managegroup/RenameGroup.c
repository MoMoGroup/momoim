#include"../ClientSockfd.h"
#include "../MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include <string.h>
#include"ManageGroup.h"
#include "../PopupWinds.h"

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

    rename_group_id = get_iter_id(data);


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
    gtk_fixed_put(GTK_FIXED(renamelayout), GTK_WIDGET(rename_done_event_box), 160, 30);


    gtk_container_add(GTK_CONTAINER (renamewindow), renameframelayout);
    gtk_container_add(GTK_CONTAINER (renameframelayout), renamelayout);
    gtk_widget_show_all(renamewindow);
    return 0;
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