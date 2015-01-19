#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint8_t key[32];
} CRPPacketNETNATRegister;

__attribute_malloc__
CRPPacketNETNATRegister *CRPNETNATRegisterCast(CRPBaseHeader *base);

int CRPNETNATRegisterSend(CRPContext context, uint32_t sessionID, uint8_t key[32]);
