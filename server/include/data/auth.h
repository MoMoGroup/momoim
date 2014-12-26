#pragma once

#include <stdint.h>

int AuthInit();

void AuthFinalize();

int AuthUser(const char *user, const unsigned char *hashKey, uint32_t *uid);