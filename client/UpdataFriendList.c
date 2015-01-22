#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <logger.h>
#include "common.h"


//新添加好友
int FriendListInsertEntry(void *data)
{
    GtkTreeIter iterGroup, iterUser;

    GdkPixbuf *pixbuf;

    CRPPacketInfoData *p = data;

//分组好友数

    uint32_t t;
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &iterGroup);//第一个分组
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &t, -1);//第一个分组id

    while (t != UGI_DEFAULT)//遍历所有分组
    {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterGroup))//没有找到分组
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &t, -1);
    }

    pixbuf = DrawFriend(&p->info, 1);//此函数返回要画的pixbuf

    gtk_tree_store_append(TreeViewListStore, &iterUser, &iterGroup);//新加一个好友iter
    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       FRIENDUID_COL, p->info.uid,
                       -1);

    g_object_unref(pixbuf);

    free(data);
    return 0;
}

//给一个uid，返回那个用户的iter
GtkTreeIter GetUserIter(uint32_t frienduid)//参数为匹配需要的uid
{
    GtkTreeIter iterUser, iterGroup;
    uint32_t uid, gid;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &iterGroup);//拿到第一个分组iter
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &gid, -1);//第一个分组id

    while (1)
    {
        if (gtk_tree_model_iter_children(GTK_TREE_MODEL(TreeViewListStore), &iterUser, &iterGroup))//拿到分组的孩子列
        {

            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterUser, FRIENDUID_COL, &uid, -1);//拿到好友id

            while (uid != frienduid)//判断是否找到
            {
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterUser))//下移一个，
                {
                    break;
                }

                gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterUser, FRIENDUID_COL, &uid, -1);//拿到好友id
            }
            if (uid == frienduid)//找到了
            {
                break;
            }

        }//应该是拿到第一个分组,第一个好友id

        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterGroup))//看是否存在下一个分组，为空说明找不到，结束循环
        {
            log_warning("GetUserIter", "Friend %u Not Found\n", frienduid);
            for (int gi = 0; gi < friends->groupCount; ++gi)
            {
                UserGroup *group = friends->groups + gi;
                log_info("Info", "Group:%s(%u)\n", group->groupName, group->groupId);
                for (int fi = 0; fi < group->friendCount; ++fi)
                {
                    log_info("Friend", "%u\n", group->friends[fi]);
                }
            }
            break;
        }

        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &uid, -1);//拿到uid
    }

    return iterUser;

}

//好友上线。图标变亮
int OnLine(void *data)
{

    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser;
    GdkPixbuf *pixbuf = NULL;

    iterUser = GetUserIter(p->uid);//拿到iter

    FriendInfo *head = FriendInfoHead;
    while (head->next)
    {
        head = head->next;
        if (p->uid == head->user.uid)
        {
            head->isonline = 1;
            pixbuf = DrawFriend(&head->user, 1);
            break;
        }
    }

    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, 1,
                       -1);
    g_object_unref(pixbuf);
    free(data);
    return 0;
}

//好友下线。图标变暗
int OffLine(void *data)
{
    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser;
    GdkPixbuf *pixbuf = NULL;

    iterUser = GetUserIter(p->uid);//拿到iter

    FriendInfo *head = FriendInfoHead;
    while (head->next)
    {
        head = head->next;
        if (p->uid == head->user.uid)
        {
            head->isonline = 0;
            pixbuf = DrawFriend(&head->user, 0);

            break;
        }
    }

    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, -1,
                       -1);
    g_object_unref(pixbuf);
    free(data);
    return 0;
}


int gengxin_ziliao(void *data)
{
    FriendInfo *infodata = data;


    GtkTreeIter iterUser = GetUserIter(infodata->uid);//拿到好友的iter
    GdkPixbuf *pixbuf = DrawFriend(&infodata->user, infodata->isonline);//重画好友头像


    log_info("更改资料", "上下线%d\n", infodata->isonline);


    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, infodata->isonline,
                       -1);

    g_object_unref(pixbuf);
    return 0;
}


//好友列表有人更新资料，实时更新到自己的列表的函数
int FriendInfoChange(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_INFO_DATA)
    {
        CRPPacketInfoData *infodata = CRPInfoDataCast(header);

        FriendInfo *p = FriendInfoHead;//遍历文件找到好友资料链表里的那个节点
        while (p->next)
        {
            p = p->next;
            if (infodata->info.uid == p->user.uid)
            {
                p->user = infodata->info;
                break;
            }
        }
        //仅仅是为了建立那个文件
        FindImage(infodata->info.icon, p, gengxin_ziliao);

        if ((void *) infodata != header->data)//释放内存
        {
            free(infodata);
        }


    }

    return 0;
}