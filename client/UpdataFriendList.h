#pragma once

#include <gtk/gtk.h>

extern int FriendListInsertEntry(void *data);

extern int OnLine(void *data);//好友上线，头像变亮的函数

extern int OffLine(void *data);//好友下线，头像变暗的函数

extern int FriendInfoChange(CRPBaseHeader *header, void *data);//好友列表有人更新资料，实时更新到自己的列表的函数

extern GtkTreeIter GetUserIter(uint32_t frienduid);//给一个uid，拿到好友iter