#pragma once

#include <pthread.h>
#include "worker.h"
#include "user.h"

void *ListenMain(void *listenSocket);

#define MAX_CLIENTS 500
extern int server_exit;
extern pthread_t ThreadListener;

#define WORKER_COUNT 8
extern WorkerType worker[WORKER_COUNT];

extern int ServerIOPoll;

void ListenerKillUser(OnlineUser *);

void UserJoinToPoll(OnlineUser *);

void UserRemoveFromPoll(OnlineUser *);