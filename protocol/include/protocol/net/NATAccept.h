#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    session_id_t session;
    uint8_t key[32];
} CRPPacketNETNATAccept;

__attribute_malloc__
CRPPacketNETNATAccept *CRPNETNATAcceptCast(CRPBaseHeader *base);

int CRPNETNATAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid, session_id_t session, uint8_t key[32]);
