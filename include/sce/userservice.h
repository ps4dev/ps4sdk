#pragma once

#include <stdint.h>
#include <sce/types/userservice.h>
int sceUserServiceInitialize(int *params);
int sceUserServiceTerminate();
int sceUserServiceGetInitialUser(SceUserServiceUserId *userId);
