#pragma once

#include <gtk/gtk.h>

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data);
extern session_id_t CountSessionId();//保护唯一的session不会冲突
