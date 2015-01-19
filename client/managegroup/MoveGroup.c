#include"../ClientSockfd.h"
#include "../MainInterface.h"
#include <logger.h>
#include <common.h>
#include"ManageGroup.h"


//分组上移……………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………
//………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………………

int up_group(void *data)
{

//遍历iter
    uint8_t gid = (uint8_t) data;
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
        g_idle_add(GroupPop, "已经是第一个分组");
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
    //  int64_t next_priority;

    uint32_t current_gid;
    uint32_t next_gid;
    GtkTreeIter up_iter_group;
    //   GtkTextIter tem_iter;


    GtkTreeView *treeview = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gtk_tree_selection_get_selected(selection, &model, &up_iter_group);//拿到选中的分组

    gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &current_gid, -1);//拿到选中的组id
    gtk_tree_model_get(model, &up_iter_group, PRIORITY_COL, &up_priority, -1);//拿到选中的权值




    if (gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &up_iter_group) != 0)
    {
        gtk_tree_model_get(model, &up_iter_group, FRIENDUID_COL, &next_gid, -1);//拿到选中的后一个权值



        session_id_t sessionid = CountSessionId();
        AddMessageNode(sessionid, down_group_recv, current_gid);
        CRPFriendGroupMoveSend(sockfd, sessionid, next_gid, current_gid);

    }
    else
    {
        g_idle_add(GroupPop, "已经是最后一个分组");

    }

    return 0;
}

