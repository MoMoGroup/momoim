#pragma once

#include <gtk/gtk.h>
#include <openssl/md5.h>

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data);

extern void Md5Coding(gchar *filename, unsigned char *coding_text);

extern int CopyFile(const char *sourceFileNameWithPath, const char *targetFileNameWithPath);

extern void HexadecimalConversion(char *filename, char *strdest);