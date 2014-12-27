#pragma once

#include "common/friends.h"

void UserGetDir(char *path, uint32_t uid, const char *relPath);

void UserCreateDirectory(uint32_t uid);

//Info
void UserCreateInfoFile(uint32_t uid, char *path);

int UserSaveInfoFile(UserInfo *info, char *path);

UserInfo *UserGetInfo(uint32_t uid);

void UserFreeInfo(UserInfo *friends);

//Groups
void UserCreateFriendsFile(char *path);

int UserSaveFriendsFile(UserFriends *friends, char *path);

UserFriends *UserGetFriends(uint32_t uid);