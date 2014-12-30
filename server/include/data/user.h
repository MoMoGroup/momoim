#pragma once

#include "imcommon/friends.h"
#include "imcommon/message.h"

int UserInit();

void UserFinalize();

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

//Info
void UserCreateInfoFile(uint32_t uid);

int UserSaveInfoFile(uint32_t uid, UserInfo *info);

UserInfo *UserGetInfo(uint32_t uid);

void UserFreeInfo(UserInfo *friends);

//Friends-Groups
void UserCreateFriendsFile(uint32_t uid);

int UserSaveFriendsFile(uint32_t uid, UserFriends *friends);

UserFriends *UserGetFriends(uint32_t uid);

void UserFreeFriends(UserFriends *friends);
