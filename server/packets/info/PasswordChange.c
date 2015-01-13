#include <run/Structures.h>
#include <run/user.h>
#include <asm-generic/errno-base.h>
#include <datafile/auth.h>

int ProcessPacketInfoPasswordChange(POnlineUser user, uint32_t session, CRPPacketInfoPasswordChange *packet)
{
    if (user->state == OUS_ONLINE)
    {
        if (user->crp->recvTd == NULL)
        {
            CRPFailureSend(user->crp, session, EINVAL, "不允许使用未加密协议修改密码");
            return 0;//要求重新登陆
        }
        if (AuthPasswordChange(user->uid, (unsigned char *) packet->password))
        {
            CRPOKSend(user->crp, session);
        }
        else
        {
            CRPFailureSend(user->crp, session, EFAULT, "未知原因");
        }
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}