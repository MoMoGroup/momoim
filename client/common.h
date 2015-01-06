#pragma once

#include <gtk/gtk.h>
#include <openssl/md5.h>

extern GtkEventBox *BuildEventBox(GtkWidget *warp, GCallback press, GCallback enter, GCallback leave, GCallback release, void *data);
extern session_id_t CountSessionId();//保护唯一的session不会冲突

extern void Md5Coding(gchar *filename, unsigned char *coding_text);

extern int CopyFile(const char *sourceFileNameWithPath, const char *targetFileNameWithPath);

extern void HexadecimalConversion(char *filename,const char *strdest);