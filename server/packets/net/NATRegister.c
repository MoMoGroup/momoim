#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <run/natTraversal.h>
#include <stdlib.h>
#include <logger.h>
#include <arpa/inet.h>
#include "run/user.h"

typedef struct
{
    uint32_t uid;
    session_id_t session;
    HostDiscoverEntry *discoverEntry;
} DiscoverOperation;

static int DiscoverCancelHandler(POnlineUser user, PUserOperation op)
{
    DiscoverOperation *discoverOperation = op->data;
    if (discoverOperation)
    {
        NatHostDiscoverUnregister(discoverOperation->discoverEntry);
        free(discoverOperation);
    }
    op->onCancel = NULL;
    UserOperationUnregister(user, op);
    return 0;
}

static int DiscoverDetected(const struct sockaddr_in *addr, void *data)
{
    PUserOperation op = data;
    DiscoverOperation *discoverOperation = op->data;
    POnlineUser user = OnlineUserGet(discoverOperation->uid);
    if (user)
    {
        struct sockaddr_in fromAddr;
        socklen_t len = sizeof(fromAddr);
        getpeername(user->crp->fd, (struct sockaddr *) &fromAddr, &len);
        log_info("Detect",
                 "UID:%u,Session:%u,From:%s\n",
                 user->uid,
                 discoverOperation->session,
                 inet_ntoa(fromAddr.sin_addr));
        CRPNATDetectedSend(user->crp, discoverOperation->session, addr->sin_addr.s_addr, addr->sin_port);
        UserDrop(user);
    }
    op->data = NULL;
    op->onCancel = NULL;
    free(discoverOperation);
    UserOperationUnregister(user, op);
    return 1;
}

int ProcessPacketNETNATRegister(POnlineUser user, uint32_t session, CRPPacketNETNATRegister *packet)
{
    if (user->state == OUS_ONLINE)
    {
        DiscoverOperation *discoverOperation = (DiscoverOperation *) malloc(sizeof(DiscoverOperation));
        discoverOperation->uid = user->uid;
        discoverOperation->session = session;
        log_info("NATRegister", "User:%u,Session:%u\n", user->uid, session);
        PUserOperation operation = UserOperationRegister(user, session, CUOT_NAT_DISCOVER, discoverOperation);
        discoverOperation->discoverEntry = NatHostDiscoverRegister(packet->key, DiscoverDetected, operation);
        operation->onCancel = DiscoverCancelHandler;
        UserOperationDrop(user, operation);
        CRPOKSend(user->crp, session);
    }
    else
    {
        CRPFailureSend(user->crp, session, EACCES, "状态错误");
    }
    return 1;
}