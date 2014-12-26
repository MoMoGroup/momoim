#pragma once

#include "server.h"

//事务队列
extern OnlineUser *jobQueue[MAX_CLIENTS];

//初始化事务管理器
void InitJobManger(void);

//从事务队列中获得一个用户
OnlineUser *PollJob(void);

//将一个用户加入到事务队列中
void PushJob(OnlineUser *);