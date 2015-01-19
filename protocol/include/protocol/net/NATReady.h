#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    session_id_t session;
} CRPPacketNETNATReady;

__attribute_malloc__
CRPPacketNETNATReady *CRPNETNATReadyCast(CRPBaseHeader *base);

int CRPNETNATReadySend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session);
