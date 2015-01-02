#pragma once

#include <stdint.h>
#include "imcommon/friends.h"
#include "imcommon/message.h"

int UserInit();

void UserFinalize();

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

int UserQueryByNick(const char *text, uint8_t page, uint8_t count, uint32_t *uids);

//Info
void UserInfoCreate(uint32_t uid);

int UserInfoSave(uint32_t uid, UserInfo *info);

UserInfo *UserInfoGet(uint32_t uid);

void UserInfoFree(UserInfo *friends);

//Friends-Groups
void UserCreateFriendsFile(uint32_t uid);

int UserSaveFriendsFile(uint32_t uid, UserFriends *friends);

UserFriends *UserGetFriends(uint32_t uid);

void UserFreeFriends(UserFriends *friends);

int UserMessageFileCreate(uint32_t uid);

MessageFile *UserMessageFileOpen(uint32_t uid);