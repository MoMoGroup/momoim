#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 同意客户端发现
*/
CRP_STRUCTURE
{
    uint32_t uid;
    session_id_t session;
} CRPPacketNETDiscoverRefuse;

__attribute_malloc__
CRPPacketNETDiscoverRefuse *CRPNETDiscoverRefuseCast(CRPBaseHeader *base);

int CRPNETDiscoverRefuseSend(CRPContext context, uint32_t sessionID, uint32_t uid,session_id_t session);