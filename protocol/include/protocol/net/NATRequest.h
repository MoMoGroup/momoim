#pragma once

#include "../base.h"
#include <stdint.h>
#include "imcommon/message.h"

typedef enum
{
    CRPNDR_AUDIO,
    CRPNDR_VEDIO,
} CRP_NAT_REASON;
CRP_STRUCTURE
{
    char key[32];
    uint32_t uid;
    uint8_t reason;
    session_id_t session;
} CRPPacketNETNATRequest;

__attribute_malloc__
CRPPacketNETNATRequest *CRPNETNATRequestCast(CRPBaseHeader *base);

int CRPNETNATRequestSend(CRPContext context,
                         uint32_t sessionID,
                         char key[32],
                         uint32_t uid,
                         uint8_t reason,
                         session_id_t session);
