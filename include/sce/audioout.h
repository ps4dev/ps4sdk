#pragma once

#include <stdint.h>
#include <types/kernel.h>
#include <types/userservice.h>



int sceAudioOutInit();
int sceAudioOutOpen(SceUserServiceUserId userId,unsigned int channel,int unknown ,unsigned int samples, unsigned int frequency, unsigned int format);
int sceAudioOutClose(int handle);
int sceAudioOutOutput(int handle, void *buf);
int sceAudioOutSetVolume(int handle, int filter, int *value);
