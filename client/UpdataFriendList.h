#pragma once

#include <gtk/gtk.h>

extern int FriendListInsertEntry(void *data);

extern int OnLine(void *data);

extern int OffLine(void *data);

extern int FriendFriendInfoChange(CRPBaseHeader *header, void *data);

extern GtkTreeIter GetUserIter(uint32_t frienduid);//给一个uid，拿到好友iter