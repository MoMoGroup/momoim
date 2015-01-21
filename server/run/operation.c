#include <run/Structures.h>
#include "run/user.h"
#include <errno.h>
#include <stdlib.h>

//在用户操作表中注册操作
// 参数:POnlineUser user - 在线用户对象
// 参数:session_id_t sessionID - 会话ID
// 参数:int type - 操作类型
// 参数:void *data - 附加数据
// 返回: 用户操作
PUserOperation UserOperationRegister(POnlineUser user, session_id_t sessionID, int type, void *data)
{
    if (user->state != OUS_ONLINE //用户需要是在线状态
        || user->operations.count >= 1000//操作数不能过多
            )
    {
        return NULL;
    }

    PUserOperation operation = (PUserOperation) calloc(1, sizeof(UserOperation));
    if (operation == NULL)
    {
        return NULL;
    }
    operation->next = NULL;
    operation->type = type;
    operation->session = sessionID;
    operation->data = data;
    pthread_mutex_init(&operation->lock, NULL);
    pthread_mutex_lock(&user->operations.lock);

    if (user->operations.last == NULL)
    {
        user->operations.first = user->operations.last = operation;
    }
    else
    {
        user->operations.last->next = operation;
        operation->prev = user->operations.last;
        user->operations.last = operation;
    }
    ++user->operations.count;

    pthread_mutex_unlock(&user->operations.lock);
    pthread_mutex_lock(&operation->lock);
    return operation;
}

//注销用户操作
// 参数:POnlineUser user - 在线用户对象
// 参数:PUserOperation op - 会话ID
//注意:调用该函数之前,请确保当前线程持有操作锁
void UserOperationUnregister(POnlineUser user, PUserOperation op)
{
    if (user->state != OUS_ONLINE)
    {
        return;
    }
    if (!op->cancel)
    {
        UserOperationCancel(user, op);
        return;
    }

    //pthread_mutex_lock(&user->operations.lock);
    if (op->prev == NULL && op->next == NULL && user->operations.first != op)
    {
        pthread_mutex_unlock(&op->lock);
        pthread_mutex_destroy(&op->lock);
        free(op);
    }
    else
    {
        if (op->prev == NULL)
        {
            user->operations.first = op->next;
        }
        else
        {
            op->prev->next = op->next;
        }
        if (op->next == NULL)
        {
            user->operations.last = op->prev;
        }
        else
        {
            op->next->prev = op->prev;
        }
        --user->operations.count;
        pthread_cond_broadcast(&user->operations.unlockCond);
        pthread_mutex_unlock(&op->lock);
        pthread_mutex_destroy(&op->lock);
        free(op);
    }
    pthread_mutex_unlock(&user->operations.lock);
}

//通过用户和会话ID得到一个操作.该函数会锁定该操作,保证只有一个线程正在使用该用户操作
// 参数:POnlineUser user - 在线用户对象
// 参数:uint32_t sessionId - 会话ID
//返回: 成功搜索到操作返回用户操作,失败返回NULL
PUserOperation UserOperationGet(POnlineUser user, uint32_t sessionId)
{
    if (user->state != OUS_ONLINE)
    {
        return NULL;
    }
    pthread_mutex_lock(&user->operations.lock);
    PUserOperation ret;
    int errcode;
    refind:
    ret = NULL;
    //找出用户操作
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if (op->session == sessionId)
        {
            ret = op;
            break;
        }
    }
    if (ret)
    {
        errcode = pthread_mutex_trylock(&ret->lock);
        if (0 != errcode)
        {
            //如果已经被锁定
            if (errcode == EBUSY)
            {
                //等待该操作被解锁,解锁后,重新寻找该用户操作
                pthread_cond_wait(&user->operations.unlockCond, &user->operations.lock);
                goto refind;
            }
            else
            {
                ret = NULL;
            }
        }
    }
    pthread_mutex_unlock(&user->operations.lock);
    return ret;
}

//解锁一个操作.允许其他线程再次获得该操作
//参数:POnlineUser user - 在线用户对象
//参数:PUserOperation op - 用户操作
void UserOperationDrop(POnlineUser user, PUserOperation op)
{
    if (user->state != OUS_ONLINE)
    {
        return;
    }
    pthread_mutex_unlock(&op->lock);
    pthread_cond_broadcast(&user->operations.unlockCond);
    if (op->cancel)
    {
        UserOperationUnregister(user, op);
    }
}

//取消一个操作.操作会被注销
//该函数会取消该操作,如果该操作有取消回调函数,该函数将被调用.
//参数:POnlineUser user - 在线用户对象
//参数:PUserOperation op - 要取消的用户操作
//返回:1
int UserOperationCancel(POnlineUser user, PUserOperation op)
{
    op->cancel = 1;
    if (op->onCancel != NULL)
    {
        op->onCancel(user, op);
    }
    UserOperationUnregister(user, op);
    return 1;
}

//注销一个用户的所有操作
//参数:POnlineUser user - 要注销的用户
void UserOperationRemoveAll(POnlineUser user)
{
    pthread_mutex_lock(&user->operations.lock);
    while (user->operations.first)
    {
        pthread_mutex_lock(&user->operations.first->lock);
        UserOperationUnregister(user, user->operations.first);
    }
    pthread_mutex_unlock(&user->operations.lock);
}
