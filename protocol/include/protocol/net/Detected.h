#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include <netinet/in.h>
#include "imcommon/message.h"

CRP_STRUCTURE
{
    uint32_t ipv4;
    uint16_t port;
} CRPPacketNATDetected;

__attribute_malloc__
CRPPacketNATDetected *CRPNATDetectedCast(CRPBaseHeader *base);

int CRPNATDetectedSend(CRPContext context, uint32_t sessionID, uint32_t ipv4, uint16_t port);
