#include"../ClientSockfd.h"
#include "../MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include"ManageGroup.h"

//好友移动所在分组……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//…………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………


typedef struct move_info
{
    //保存移动好友需要的信息
    uint32_t currentuid;
    uint32_t currentGid;
    uint32_t toGid;
    GtkTreeIter itergroup;
    GtkTreeIter iteruser;


} move_info;


gboolean mov(void *data)
{

    move_info *move_info_detail = (move_info *) data;

    GtkTreeIter iteruser;
    GdkPixbuf *pixbuf;
    uint32_t uid;
    int64_t priority;
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &move_info_detail->iteruser,
                       PIXBUF_COL, &pixbuf,
                       FRIENDUID_COL, &uid,
                       PRIORITY_COL, &priority,
                       -1
                      );


    gtk_tree_store_append(TreeViewListStore, &iteruser, &move_info_detail->itergroup);
    gtk_tree_store_set(TreeViewListStore, &iteruser,
                       PIXBUF_COL, pixbuf,
                       FRIENDUID_COL, uid,
                       PRIORITY_COL, priority,
                       -1);

    UserGroup *frome = UserFriendsGroupGet(friends, move_info_detail->currentGid);
    UserGroup *to = UserFriendsGroupGet(friends, move_info_detail->toGid);
    UserFriendsUserMove(frome, to, uid);

    gtk_tree_store_remove(TreeViewListStore, &move_info_detail->iteruser);


    return 0;
}

int mov_recv(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_OK)
    {
        g_idle_add(mov, data);
    }
    else
    {

        g_idle_add(GroupPop, "移动失败");
    }

    return 0;
}


int mov_friend(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    move_info *move_info_detail = g_object_get_data(G_OBJECT(widget), "moveInfo");

    // uint8_t current_friend_uid = (uint8_t *) data;
    session_id_t sessionid = CountSessionId();
    AddMessageNode(sessionid, mov_recv, move_info_detail);
    CRPFriendMoveSend(sockfd,
                      sessionid,
                      move_info_detail->currentuid,
                      move_info_detail->currentGid,
                      move_info_detail->toGid);
    return 0;
}


GtkWidget *MovFriendButtonEvent(GtkTreeView *treeview)
{
    move_info moveInfo;
    uint32_t curren_group_id;
    GtkWidget *show;
    uint32_t groupid;
    GtkTreeIter itergroup, iteruser;
//首先拿到选中好友所在分组id,和好友uid
    uint32_t current_friend_uid;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &iteruser);//拿到选中的列


    gtk_tree_model_get(model, &iteruser, FRIENDUID_COL, &current_friend_uid, -1);//选中好友uid

    gtk_tree_model_iter_parent(GTK_TREE_MODEL(TreeViewListStore), &itergroup, &iteruser);//拿到父列
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &itergroup, FRIENDUID_COL, &curren_group_id, -1);
    log_info("curren_group_id", "%d\n", curren_group_id);
    log_info("current_friend_uid", "%d\n", current_friend_uid);

    moveInfo.currentuid = current_friend_uid;//选中好友uid
    moveInfo.currentGid = curren_group_id;//选择的组id
    moveInfo.iteruser = iteruser;//好友iter

    // gtk_tree_model_iter_parent(<#(GtkTreeModel*)tree_model#>, <#(GtkTreeIter*)iter#>, <#(GtkTreeIter*)child#>)
    UserGroup *userCueentGroup = UserFriendsGroupGet(friends, curren_group_id);


    GtkWidget *mov_menu = gtk_menu_new();//创建一个菜单
//遍历所有分组
    UserGroup *friendGroup;
    GtkTreeIter to_group_iter;
    //第一个分组id的信息

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &to_group_iter);

    do
    {
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &to_group_iter,
                           FRIENDUID_COL, &groupid,
                           -1);

        move_info *move_info_detail = (move_info *) malloc(sizeof(move_info));
        *move_info_detail = moveInfo;
        move_info_detail->itergroup = to_group_iter;//to分组iter
        move_info_detail->toGid = groupid;

        friendGroup = UserFriendsGroupGet(friends, groupid);//保存好友信息
        show = gtk_menu_item_new_with_mnemonic(friendGroup->groupName);//显示名字
        gtk_container_add(GTK_CONTAINER(mov_menu), show);//添加到菜单

        g_object_set_data_full(G_OBJECT(show), "moveInfo", move_info_detail, free);
        g_signal_connect(G_OBJECT(show), "button_press_event",
                         G_CALLBACK(mov_friend), current_friend_uid);

        gtk_widget_show(show);
    }
    while (gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &to_group_iter));
    return mov_menu;
}