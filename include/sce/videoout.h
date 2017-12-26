#pragma once

#include <stdint.h>
#include <types/kernel.h>
#include <types/userservice.h>
#include <types/videoout.h>



int sceVideoOutOpen(SceUserServiceUserId userId, int type, int index, const void *param);
int sceVideoOutClose(int handle);
int sceVideoOutRegisterBuffers(int handle, int initialIndex, void * const *addr, int numBuf, const SceVideoOutBufferAttribute *attr);
int sceVideoOutUnregisterBuffers(int handle, int indexAttr);
int sceVideoOutSubmitFlip(int handle, int indexBuf, unsigned int flipMode, int64_t flipArg);
void sceVideoOutSetBufferAttribute(SceVideoOutBufferAttribute *attr, unsigned int format, unsigned int tmode,unsigned int aspect, unsigned int width, unsigned int height, unsigned int pixelPitch);
int sceVideoOutSetFlipRate(int handle, int flipRate);
int sceVideoOutAddFlipEvent(SceKernelEqueue eq, int handle, void *data);
int sceVideoOutGetFlipStatus(int handle, SceVideoOutFlipStatus *status);
