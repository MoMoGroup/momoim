#pragma once

#include <stdint.h>

int AuthInit();

void AuthFinalize();

int AuthUser(const char *user, const unsigned char *hashKey, int *uid);