#pragma once

#include <sce/types/kernel.h>
#include <stddef.h>
#include <sys/types.h>

/* useless C / POSIX wrappers */
int sceKernelGetCurrentCpu(void);
int sceKernelGettimeofday(SceKernelTimeval *tp);
int sceKernelUsleep(unsigned int microseconds);
int scePthreadCancel(ScePthread thread);
int scePthreadCreate(ScePthread *thread, const ScePthreadAttr *attr, void *(*entry)(void *), void *arg, const char *name);
int scePthreadDetach(ScePthread thread);
int scePthreadJoin(ScePthread thread, void **value_ptr);
int scePthreadMutexDestroy(ScePthreadMutex *mutex);
int scePthreadMutexInit(ScePthreadMutex *mutex, const ScePthreadMutexattr *attr, const char *name);
int scePthreadMutexLock(ScePthreadMutex *mutex);
int scePthreadMutexTimedlock(ScePthreadMutex *mutex, SceKernelUseconds usec);
int scePthreadMutexTrylock(ScePthreadMutex *mutex);
int scePthreadMutexUnlock(ScePthreadMutex *mutex);
ScePthread scePthreadSelf(void);
uint64_t sceKernelGetProcessTime(void);
unsigned int sceKernelSleep(unsigned int seconds);
void scePthreadExit(void *value);
void scePthreadYield(void);

int sceKernelCreateEqueue(SceKernelEqueue *eq, const char *name);
int sceKernelDeleteEqueue(SceKernelEqueue eq);
int sceKernelAddUserEvent(SceKernelEqueue eq, int id);
int sceKernelAddReadEvent(SceKernelEqueue eq, int fd, size_t size, void *data);
int sceKernelWaitEqueue(SceKernelEqueue eq, SceKernelEvent *ev, int num, int *out, SceKernelUseconds *timo);   

size_t sceKernelGetDirectMemorySize();

/* more interresting wrappers */
int sceKernelAllocateDirectMemory(off_t searchStart, off_t searchEnd, size_t length, size_t alignment, int type, off_t *physicalAddressDestination);
int sceKernelMapDirectMemory(void **addr, size_t length, int protection, int flags, off_t start, size_t alignment);

/* sce own syscall wrappers (usefull) */
int sceKernelLoadStartModule(const char *name, size_t argc, const void *argv, uint32_t flags, void *, int *result);
int sceKernelDlsym(SceKernelModule handle, const char *symbol, void **address);
int sceKernelGetModuleList(SceKernelModule *array, size_t size, size_t *available);
int sceKernelGetModuleInfo(SceKernelModule handle, SceKernelModuleInfo *info);
int sceKernelStopUnloadModule(SceKernelModule handle, size_t argc, const void *argv, uint32_t flags, void *, int *result);

int sceKernelJitCreateSharedMemory(int flags, size_t size, int protection, int *destinationHandle);
int sceKernelJitCreateAliasOfSharedMemory(int handle, int protection, int *destinationHandle);
int sceKernelJitMapSharedMemory(int handle, int protection, void **destination);
