#pragma once

extern int ChangeInfo();

struct HeadPictureData
{
    size_t seq;
    FILE *fp;
    uint32_t uid;
    UserInfo user;
    unsigned char *code;
};