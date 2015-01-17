#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

//enum {
//    PIXBUF_COL = 0,
//    FRIENDUID_COL = 1,
//};


//新添加好友
int FriendListInsertEntry(void *data)
{
    GtkTreeIter iterGroup, iterUser;

    GdkPixbuf *pixbuf;
//
    CRPPacketInfoData *p = data;

//分组好友数

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &iterGroup);//第一个分组
    uint32_t t;
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &t, -1);//第一个分组id

    while (t != UGI_DEFAULT)
    {
        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterGroup))//没有找到分组
        {
            break;
        }
        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &t, -1);
    }

    pixbuf = DrawFriend(&p->info, 1);

    gtk_tree_store_append(TreeViewListStore, &iterUser, &iterGroup);//
    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       FRIENDUID_COL, p->info.uid,
                       -1);

    g_object_unref(pixbuf);

    // return GTK_TREE_MODEL(TreeViewListStore);

    free(data);
    return 0;
}

//拿到那个用户的iter
GtkTreeIter getUserIter(uint32_t frienduid)//参数为匹配需要的uid
{
    GtkTreeIter iterUser, iterGroup;
    uint32_t uid, gid;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(TreeViewListStore), &iterGroup);//拿到第一个
    gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &gid, -1);//第一个分组id

    while (1)
    {
        if (gtk_tree_model_iter_children(GTK_TREE_MODEL(TreeViewListStore), &iterUser, &iterGroup))//
        {

            gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterUser, FRIENDUID_COL, &uid, -1);//拿到好友id

            while (uid != frienduid)
            {
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterUser))
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

        if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(TreeViewListStore), &iterGroup))
        {
            break;
        }

        gtk_tree_model_get(GTK_TREE_MODEL(TreeViewListStore), &iterGroup, FRIENDUID_COL, &uid, -1);//拿到id
    }

    return iterUser;

}

int OnLine(void *data)//好友上线。图标变亮
{

    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser;
    GdkPixbuf *pixbuf = NULL;

    iterUser = getUserIter(p->uid);//拿到iter

    FriendInfo *head = FriendInfoHead;
    while (head->next)
    {
        head = head->next;
        if (p->uid == head->user.uid)
        {

            pixbuf = DrawFriend(&head->user, 1);
            break;
        }
    }

    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, 1,
                       -1);
    g_object_unref(pixbuf);
    return 0;
}

int OffLine(void *data)//好友下线。图标变暗
{
    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser;
    GdkPixbuf *pixbuf = NULL;

    iterUser = getUserIter(p->uid);//拿到iter

    FriendInfo *head = FriendInfoHead;
    while (head->next)
    {
        head = head->next;
        if (p->uid == head->user.uid)
        {

            pixbuf = DrawFriend(&head->user, 0);
            break;
        }
    }

    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, -1,
                       -1);
    g_object_unref(pixbuf);
    return 0;
}


int gengxinjiemian(void *data)
{
    CRPPacketInfoData *infodata = data;

    GtkTreeIter iterUser;

    iterUser = getUserIter(infodata->info.uid);

    GdkPixbuf *pixbuf;
    pixbuf = DrawFriend(&infodata->info, infodata->isOnline);
    gtk_tree_store_set(TreeViewListStore, &iterUser,
                       PIXBUF_COL, pixbuf,
                       PRIORITY_COL, infodata->isOnline,
                       -1);
    g_object_unref(pixbuf);
    return 0;
}

int FriendFriendInfoChange(CRPBaseHeader *header, void *data)
{
    if (header->packetID == CRP_PACKET_INFO_DATA)
    {
        CRPPacketInfoData *infodata = CRPInfoDataCast(header);





        //仅仅是为了建立那个文件
        FindImage(infodata->info.icon, NULL, NULL);

        FriendInfo *p = FriendInfoHead;//更新链表里的资料
        while (p->next)
        {
            p = p->next;
            if (infodata->info.uid == p->user.uid)
            {
                memcpy(p->user.nickName, infodata->info.nickName, sizeof(infodata->info.nickName));
                memcpy(p->user.icon, infodata->info.icon, sizeof(infodata->info.icon));
                break;
            }
        }

        g_idle_add(gengxinjiemian, infodata);


    }

    return 0;
}