#include <gtk/gtk.h>
#include <protocol/info/Data.h>
#include "MainInterface.h"
#include <math.h>
#include <stdlib.h>
#include <imcommon/friends.h>
#include "common.h"

enum {
    PIXBUF_COL = 0,
    FRIENDUID_COL = 1,
};



void upda_first(void *data)
{
    GtkTreeIter iterGroup, iterUser;

    GdkPixbuf *pixbuf;
//
    CRPPacketInfoData *p = data;

//分组好友数
    int num = friends->groups[0].friendCount;

    gtk_tree_model_get_iter_first(TreeViewListStore, &iterGroup);
    uint32_t t;
    gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &t, -1);

    while (t != UGI_DEFAULT)
    {
        if (!gtk_tree_model_iter_next(TreeViewListStore, &iterGroup))
        {
            abort();
        }
        gtk_tree_model_get(TreeViewListStore, &iterGroup, FRIENDUID_COL, &t, -1);
    }

    pixbuf=draw(data);

    gtk_tree_store_append(TreeViewListStore, &iterUser, &iterGroup);//
    gtk_tree_store_set(TreeViewListStore, &iterUser,
            PIXBUF_COL, pixbuf,
            FRIENDUID_COL,p->info.uid,
            -1);

    g_object_unref(pixbuf);


}