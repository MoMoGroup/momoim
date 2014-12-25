#pragma once

#include "server.h"

extern OnlineUser *jobQueue[MAX_CLIENTS];
extern sem_t SemaphoreJobQueue;

void InitJobManger(void);

OnlineUser *PollJob(void);

void PushJob(OnlineUser *);