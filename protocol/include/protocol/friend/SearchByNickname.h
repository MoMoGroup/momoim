#pragma once

#include <protocol/base.h>
#include <stdint.h>

/**
* 请求好友列表
*/

CRP_STRUCTURE {
    uint8_t page;
    uint8_t size;
    char text[0];
} CRPPacketFriendSearchByNickname;

__attribute_malloc__
CRPPacketFriendSearchByNickname *CRPFriendSearchByNicknameCast(CRPBaseHeader *base);

int CRPFriendSearchByNicknameSend(CRPContext context, uint32_t sessionID, uint8_t page, uint8_t size, const char *text);