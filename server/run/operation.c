#include <run/Structures.h>
#include "run/user.h"
#include <errno.h>
#include <stdlib.h>

PUserOperation UserOperationRegister(POnlineUser user, session_id_t sessionID, int type, void *data)
{
    /* It will cause some trouble */
    if (user->operations.count >= 1000)
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

void UserOperationUnregister(POnlineUser user, PUserOperation op)
{
    if (!op->cancel)
    {
        UserOperationCancel(user, op);
        return;
    }

    pthread_mutex_lock(&user->operations.lock);
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
        pthread_mutex_unlock(&op->lock);
        pthread_cond_broadcast(&user->operations.unlockCond);
        pthread_mutex_destroy(&op->lock);
        free(op);
    }
    pthread_mutex_unlock(&user->operations.lock);
}

PUserOperation UserOperationGet(POnlineUser user, uint32_t sessionId)
{
    pthread_mutex_lock(&user->operations.lock);
    PUserOperation ret;
    int errcode;
    refind:
    ret = NULL;
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
            if (errcode == EBUSY)
                //本机测试pthread_mutex_trylock返回非0值的时候,errno返回竟然是0.Unbelievable!
            {
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

void UserOperationDrop(POnlineUser user, PUserOperation op)
{
    pthread_mutex_unlock(&op->lock);
    pthread_cond_broadcast(&user->operations.unlockCond);
}

PUserOperation UserOperationQuery(POnlineUser user,
                                  UserOperationType type,
                                  int (*func)(PUserOperation op, void *data),
                                  void *data)
{
    pthread_mutex_lock(&user->operations.lock);
    PUserOperation ret = NULL;
    for (PUserOperation op = user->operations.first; op != NULL; op = op->next)
    {
        if ((type == -1 || op->type == type) && func(op, data))
        {
            ret = op;
            break;
        }
    }
    pthread_mutex_unlock(&user->operations.lock);
    if (ret)
    {
        if (pthread_mutex_trylock(&ret->lock))
        {
            return NULL;
        }
    }
    return ret;
}

int UserOperationCancel(POnlineUser user, PUserOperation op)
{
    op->cancel = 1;
    if (op->onCancel != NULL)
    {
        if (op->onCancel(user, op))
        {
            UserOperationDrop(user, op);
        }
    }
    else
    {
        UserOperationUnregister(user, op);
    }
    return 1;
}

void UserOperationRemoveAll(POnlineUser user)
{
    if (pthread_mutex_lock(&user->operations.lock))
    {
        abort();
    }
    PUserOperation next = user->operations.first;
    user->operations.first = user->operations.last = NULL;
    for (PUserOperation op = next; op != NULL; op = next)
    {
        next = op->next;
        op->prev = op->next = NULL;
        UserOperationCancel(user, op);
    }
    user->operations.count = 0;
    pthread_mutex_unlock(&user->operations.lock);
}
