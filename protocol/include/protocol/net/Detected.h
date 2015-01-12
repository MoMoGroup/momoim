#pragma once

#include "protocol/base.h"
#include <stdint.h>
#include <netinet/in.h>
#include "imcommon/message.h"

CRP_STRUCTURE {
    struct sockaddr_in addr;
} CRPPacketNATDetected;

__attribute_malloc__
CRPPacketNATDetected *CRPNATDetectedCast(CRPBaseHeader *base);

int CRPNATDetectedSend(CRPContext context, uint32_t sessionID, const struct sockaddr_in *addr);
