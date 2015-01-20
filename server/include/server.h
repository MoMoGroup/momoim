#pragma once

#include <pthread.h>
#include <sqlite3.h>
#include "config.h"
#include "run/worker.h"
#include "run/user.h"

//socket监听线程主函数
void *ListenMain(void *);

//服务端是否正在退出
extern int IsServerRunning;
//监听线程
extern pthread_t ThreadListener;

//将一个用户加入到epoll中
void EpollAdd(POnlineUser);

//将一个用户从epoll中移除
void EpollRemove(POnlineUser);

//重新设置fd事件
void EpollModify(POnlineUser);
void BanListAdd(in_addr_t ip);
void BanListRemove(in_addr_t ip);