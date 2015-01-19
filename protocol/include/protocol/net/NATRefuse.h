#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    session_id_t session;
} CRPPacketNETNATRefuse;

__attribute_malloc__
CRPPacketNETNATRefuse *CRPNETNATRefuseCast(CRPBaseHeader *base);

int CRPNETNATRefuseSend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session);
