#pragma once

#include <glob.h>

typedef enum
{
    IOT_READ,
    IOT_WRITE,

} IOOperationType;
struct structIOOperation
{
    IOOperationType type;
    volatile void *buf;
    size_t nbytes;
};

//初始化IO管理器
void InitIOManger(void);

//将一个IO操作从IO管理器中移除
void IOManagerKick(PIOOperation operation);

//从IO队列中获得一个用户
PIOOperation IOManagerPop(void);

//将一个用户加入到事务队列中
void IOManagerPush(PIOOperation);
