#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include <netinet/in.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    struct sockaddr_in addr;
} CRPPacketP2PDetected;

__attribute_malloc__
CRPPacketP2PDetected *CRPP2PDetectedCast(CRPBaseHeader *base);

int CRPP2PDetectedSend(CRPContext context, uint32_t sessionID, const struct sockaddr_in *addr);
