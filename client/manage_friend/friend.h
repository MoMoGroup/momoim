#pragma once

#include <gtk/gtk.h>

#include "../MainInterface.h"

extern int Friend_Delete_Popup(GtkTreeView *treeview);
extern int RemoveFriend(void *data);

extern int AddFriendFun();

extern int Friend_Request_Popup(uint32_t uid, const char *verification_message);