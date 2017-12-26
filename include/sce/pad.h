#pragma once

#include <stdint.h>
#include <sce/types/pad.h>
#include <sce/types/userservice.h>

int scePadInit(void);
int scePadOpen(SceUserServiceUserId userId,int type,int index,uint8_t *param);
int scePadClose(int handle);
int scePadRead(int handle, ScePadData *data, int count);
int scePadReadState(int handle, ScePadData *data);

