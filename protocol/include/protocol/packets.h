#pragma once

#include "protocol/status/Hello.h"
#include "protocol/status/KeepAlive.h"
#include "protocol/status/OK.h"
#include "protocol/login/LoginFailure.h"
#include "protocol/login/Logout.h"
#include "protocol/message/TextMessage.h"

typedef enum
{
    CRP_PACKET_KEEP_ALIVE = 0,
    CRP_PACKET_HELLO,
    CRP_PACKET_OK,
    CRP_PACKET_LOGIN__START = 0x10,
    CRP_PACKET_LOGIN_FAILURE,
    CRP_PACKET_LOGIN_LOGOUT,
    CRP_PACKET_MESSAGE__START = 0x20,
    CRP_PACKET_MESSAGE_TEXT,
    CRP_PACKET_CRASH = 0xffff
} CRPPacketIDs;