#pragma once

#include "server.h"

//初始化事务管理器
void InitJobManger(void);

//将一个用户从事务管理器中移除
void JobManagerKick(POnlineUser user);

//从事务队列中获得一个用户
POnlineUser JobManagerPop(void);

//将一个用户加入到事务队列中
void JobManagerPush(POnlineUser);