#pragma once

#include <gtk/gtk.h>

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data);
