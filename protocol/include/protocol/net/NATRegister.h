#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    char key[32];
} CRPPacketNETNATRegister;

__attribute_malloc__
CRPPacketNETNATRegister *CRPNETNATRegisterCast(CRPBaseHeader *base);

int CRPNETNATRegisterSend(CRPContext context, uint32_t sessionID, char key[32]);
