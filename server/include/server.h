#pragma once

#include <pthread.h>
#include <sqlite3.h>
#include "worker.h"
#include "user.h"

extern int userdb;

//socket监听线程主函数
void *ListenMain(void *listenSocket);

//最大客户端数
#define MAX_CLIENTS 500
//服务端是否正在退出
extern int server_exit;
//监听线程
extern pthread_t ThreadListener;

//事务处理线程数
#define WORKER_COUNT 8
//事务处理器
extern WorkerType worker[WORKER_COUNT];
//epoll文件描述符
extern int ServerIOPoll;

//将一个用户加入到epoll中
void UserJoinToPoll(OnlineUser *);

//将一个用户从epoll中移除
void UserRemoveFromPoll(OnlineUser *);