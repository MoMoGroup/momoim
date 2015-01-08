#pragma once

#include <gtk/gtk.h>
#include <openssl/md5.h>
#include "protocol.h"

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release,
                                  GCallback click, void *data);

extern session_id_t CountSessionId();//保护唯一的session不会冲突

extern void Md5Coding(gchar *filename, const unsigned char *coding_text);

extern int CopyFile(const char *sourceFileNameWithPath, const char *targetFileNameWithPath);

extern void HexadecimalConversion(char *filename, const char *strdest);

extern void HexadecimalConversion(char *filename, const char *strdest);

extern int FindImage(const char *key, const void *data, gboolean (*fn)(void *data));

GdkPixbuf *draw(void *data);