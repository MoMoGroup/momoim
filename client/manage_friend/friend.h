#pragma once

#include <gtk/gtk.h>

#include "../MainInterface.h"

//extern int Friend_Delete_Popup(GtkTreeView *treeview);
extern int Friend_Delete_Popup(GtkWidget *widget, GdkEventButton *event, GtkTreeView *treeview);

extern int RemoveFriend(void *data);

extern int AddFriendFun();

//新好友验证消息
extern int FriendRequestPopup(uint32_t uid, const char *verification_message);