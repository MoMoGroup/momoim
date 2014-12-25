#pragma once

#include <user.h>
#include <protocol/CRPPackets.h>

int ProcessPacketStatusHello(OnlineUser *user, CRPPacketHello *packet);

int(*PacketsMap[])(OnlineUser *user, CRPPacketHello *packet) = {
        [CRP_PACKET_HELLO]=ProcessPacketStatusHello
};