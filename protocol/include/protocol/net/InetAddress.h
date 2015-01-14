#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t uid;
    uint32_t ipv4;
} CRPPacketNETInetAddress;

__attribute_malloc__
CRPPacketNETInetAddress *CRPNETInetAddressCast(CRPBaseHeader *base);

int CRPNETInetAddressSend(CRPContext context, uint32_t sessionID, uint32_t uid, uint32_t ipv4);
