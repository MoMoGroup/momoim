#pragma once

//extern void* primary_video(struct sockaddr_in*);

void StartVideoChat(struct sockaddr_in *addr,int (*update_flag)(),int(*pupup_window)(char const*,char const*));

void pre_closewindow();