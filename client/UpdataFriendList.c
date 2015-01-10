#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <stdlib.h>
#include <lber.h>
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

    gtk_tree_model_get_iter_first(TreeViewListStore, &iterGroup);//第一个分组
    uint32_t t;
    gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &t, -1);//第一个分组id

    while (t != UGI_DEFAULT)
    {
        if (!gtk_tree_model_iter_next(TreeViewListStore, &iterGroup))//没有找到分组
        {
            abort();
        }
        gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &t, -1);
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

int OnLine(void *data)
{

    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser, iterGroup;
    GdkPixbuf *pixbuf = NULL;

    gtk_tree_model_get_iter_first(TreeViewListStore, &iterGroup);
    uint32_t use, usg;

    gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &usg, -1);//第一个分组id



    while (1)
    {

        if (gtk_tree_model_iter_children(TreeViewListStore, &iterUser, &iterGroup))
        {

            gtk_tree_model_get(TreeViewListStore, &iterUser, FRIENDUID_COL, &use, -1);//拿到好友id

            while (use != p->uid)
            {
                if (!gtk_tree_model_iter_next(TreeViewListStore, &iterUser))
                {
                    break;
                }

                gtk_tree_model_get(TreeViewListStore, &iterUser, FRIENDUID_COL, &use, -1);//拿到好友id
            }
            if (use == p->uid)//找到了
            {
                break;
            }

        }//应该是拿到第一个分组,第一个好友id

        if (!gtk_tree_model_iter_next(TreeViewListStore, &iterGroup))
        {
            abort();
        }

        gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &usg, -1);//拿到id
    }
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
    return 0;
}

int OffLine(void *data)
{
    CRPPacketFriendNotify *p = data;
    //找到这个好友
    GtkTreeIter iterUser, iterGroup;
    GdkPixbuf *pixbuf = NULL;

    gtk_tree_model_get_iter_first(TreeViewListStore, &iterGroup);
    uint32_t use, usg;

    gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &usg, -1);//第一个分组id



    while (1)
    {

        if (gtk_tree_model_iter_children(TreeViewListStore, &iterUser, &iterGroup))
        {

            gtk_tree_model_get(TreeViewListStore, &iterUser, FRIENDUID_COL, &use, -1);//拿到好友id

            while (use != p->uid)
            {
                if (!gtk_tree_model_iter_next(TreeViewListStore, &iterUser))
                {
                    break;
                }

                gtk_tree_model_get(TreeViewListStore, &iterUser, FRIENDUID_COL, &use, -1);//拿到好友id
            }
            if (use == p->uid)//找到了
            {
                break;
            }

        }//应该是拿到第一个分组,第一个好友id

        if (!gtk_tree_model_iter_next(TreeViewListStore, &iterGroup))
        {
            break;
        }

        gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &usg, -1);//拿到id
    }
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

    return 0;
}