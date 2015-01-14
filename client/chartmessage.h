#pragma once

extern void SendText(FriendInfo *info);

extern void ShoweRmoteText(const gchar *rcvd_text, FriendInfo *info, uint16_t len);

extern void CodingWordColor(FriendInfo *info, gchar *coding, int *count);

extern void UploadingFile(gchar *filename, FriendInfo *info);

struct PictureMessageFileUploadingData
{
    size_t seq;
    FILE *fp;
    int file_loading_end;
    int file_count;
    int file_size;
    GtkWidget *file;
    GtkWidget *progressbar;
    FriendInfo *info;
    struct ImageMessageFileData *image_message_data;
};

struct ImageMessageFileData
{
    int imagecount;
    int charlen;
    char *message_data;
    uint32_t uid;

};
