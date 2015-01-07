#pragma once

extern void ShoweRmoteText(const gchar *rcvd_text, friendinfo *info, uint16_t len);

struct PictureMessageFileUploadingData
{
    char Key[16];
    size_t seq;
    FILE *fp;
    struct ImageMessageFileData *imagemessagedata;
};

struct ImageMessageFileData
{
    int imagecount;
    int charlen;
    char *message_data;
    uint32_t uid;
};