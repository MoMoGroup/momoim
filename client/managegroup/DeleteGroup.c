#include"../ClientSockfd.h"
#include "../MainInterface.h"
#include<stdlib.h>
#include <logger.h>
#include <common.h>
#include"ManageGroup.h"
#include "../PopupWinds.h"
//删除分组………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

uint8_t del_usg;
GtkTreeIter del_iterGroup;

int GroupPop(void *data)
{
    popup("系统消息", data);
    return 0;
}

int delete_group_recv(CRPBaseHeader *header, void *data)
{

    if (header->packetID == CRP_PACKET_OK)
    {
        gtk_tree_store_remove(TreeViewListStore, &del_iterGroup);
        UserFriendsGroupDelete(friends, del_usg);
        return 0;
    }
    else
    {
        g_idle_add(GroupPop, "删除失败");
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
