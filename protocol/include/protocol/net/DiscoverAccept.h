#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 同意客户端发现
*/
CRP_STRUCTURE
{
    uint32_t uid;
} CRPPacketNETDiscoverAccept;

__attribute_malloc__
CRPPacketNETDiscoverAccept *CRPNETDiscoverAcceptCast(CRPBaseHeader *base);

int CRPNETDiscoverAcceptSend(CRPContext context, uint32_t sessionID, uint32_t uid);