#pragma once

#include <gtk/gtk.h>
#include <openssl/md5.h>
#include "protocol.h"

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release,
                                  GCallback click, void *data);

extern session_id_t CountSessionId();//保护唯一的session不会冲突

extern void Md5Coding(const gchar *filename, unsigned char *coding_text);

extern int CopyFile(const char *sourceFileNameWithPath, const char *targetFileNameWithPath);

extern void HexadecimalConversion(char *filename, unsigned char const *fileKey);

extern void FindImage(const char *key, const void *data, gboolean (*fn)(void *data));

extern GdkPixbuf *DrawFriend(const UserInfo *userInfo, int draw_color);

extern cairo_surface_t *ChangeThem_png(char *picname);

extern GtkWidget *ChangeThem_file(char *picname);

extern GtkWidget *ChangeFace_file(char *picname);

extern void PlayMusic(const char *data);


