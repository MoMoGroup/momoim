#pragma once

#include <pthread.h>
#include <sqlite3.h>
#include "run/worker.h"
#include "run/user.h"

//socket监听线程主函数
void *ListenMain(void *listenSocket);

//最大客户端数
#define MAX_CLIENTS 500
//服务端是否正在退出
extern int server_exit;
//监听线程
extern pthread_t ThreadListener;

//事务处理线程数
#define WORKER_COUNT 16
//事务处理器
extern WorkerType worker[WORKER_COUNT];

//将一个用户加入到epoll中
void UserJoinToPool(OnlineUser *);

//将一个用户从epoll中移除
void UserRemoveFromPool(OnlineUser *);