#include <protocol/CRPPackets.h>
#include <asm-generic/errno-base.h>
#include <run/natTraversal.h>
#include <stdlib.h>
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
    NatHostDiscoverUnregister(discoverOperation->discoverEntry);
    free(discoverOperation);
    op->onCancel = NULL;
    UserOperationUnregister(user, op);
    return 0;
}

static void DiscoverDetected(struct sockaddr_in *addr, void *data)
{
    PUserOperation op = data;
    DiscoverOperation *discoverOperation = op->data;
    POnlineUser user = OnlineUserGet(discoverOperation->uid);
    if (user)
    {
        CRPNATDetectedSend(user->sockfd, discoverOperation->session, addr);
        UserDrop(user);
    }
    free(discoverOperation);
    op->onCancel = NULL;
    UserOperationUnregister(user, op);
}

int ProcessPacketNatDiscover(POnlineUser user, uint32_t session, CRPPacketNATDiscover *packet)
{
    if (user->status == OUS_ONLINE)
    {
        DiscoverOperation *discoverOperation = (DiscoverOperation *) malloc(sizeof(DiscoverOperation));
        discoverOperation->uid = user->info->uid;
        discoverOperation->session = session;
        discoverOperation->discoverEntry = NatHostDiscoverRegister(packet->key, DiscoverDetected, discoverOperation);
        PUserOperation operation = UserOperationRegister(user, session, CUOT_NAT_DISCOVER, discoverOperation);
        operation->onCancel = DiscoverCancelHandler;
        UserOperationDrop(user, operation);
        CRPOKSend(user->sockfd, session);
    }
    else
    {
        CRPFailureSend(user->sockfd, session, EACCES, "状态错误");
    }
    return 1;
}