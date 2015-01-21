#pragma once

//extern void* primary_video(struct sockaddr_in*);

extern void StartVideoChat(struct sockaddr_in *addr,int (*update_flag)());

void pre_closewindow();