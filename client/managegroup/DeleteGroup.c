#include"ClientSockfd.h"
#include "MainInterface.h"
#include<stdlib.h>
#include <common.h>
#include"ManageGroup.h"
#include "PopupWinds.h"
//删除分组………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

uint32_t del_gid;
GtkTreeIter del_iterGroup;

int GroupPop(void *data)
{
    Popup("系统消息", data);
    return 0;
}

//接收服务器，判断是否删除成功
int delete_group_recv(CRPBaseHeader *header, void *data)
{

    if (header->packetID == CRP_PACKET_OK)
    {
        gtk_tree_store_remove(TreeViewListStore, &del_iterGroup);//从列表中一处
        UserFriendsGroupDelete(friends, (uint8_t) del_gid);//从friends对象中删除
        g_idle_add(GroupPop, "删除分组成功");//成功弹窗提醒
        return 0;
    }
    else
    {
        g_idle_add(GroupPop, "删除分组失败");//失败弹窗提醒
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
        gtk_tree_model_get(model, &del_iterGroup, FRIENDUID_COL, &del_gid, -1);//拿到分组id，保存到del_usg

        if (del_gid == 0 || del_gid == 1)//看是否为默认分组
        {
            Popup("系统消息", "不能删除默认分组");
        }
        else
        {
            session_id_t sessionid = CountSessionId();//注册会话接受服务器
            AddMessageNode(sessionid, delete_group_recv, NULL);
            CRPFriendGroupDeleteSend(sockfd, sessionid, (uint8_t) del_gid);//删除分组请求

        }

    }
    else//非空，不能删除
    {
        Popup("系统消息", "不能删除非空分组");
    }

    return 0;
}
