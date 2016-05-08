#pragma once

#include <sys/sysent.h>
#include <stdint.h>

// call
// extension signature, base for kernel execution
int ps4KernelExecute(void *call, void *uap, int64_t *ret0, int64_t *ret1);
// Omnislash - 9999, 9999, .... - Mimic - ...
int64_t ps4KernelCall(void *call, ...);
int64_t ps4KernelCallBase(void *call, int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9);

// info & util
int ps4KernelBrewCoffee();
void *ps4KernelDlSym(char *name);
int ps4KernelIsInKernel();
int ps4KernelIsKernelAddress(void *addr);

// sec stuff
int ps4KernelUartEnable();
int ps4KernelUartDisable();

void ps4KernelPrivilegeRoot();
int ps4KernelPrivilegeUnjail();
int ps4KernelPrivilegeEscalate();

// memory
int ps4KernelMemoryAllocate(void **memory, size_t size);
void *ps4KernelMemoryMalloc(size_t size);
void ps4KernelMemoryFree(void *addr);
void ps4KernelMemoryCopy(void *src, void *dest, size_t size);

int ps4KernelAssemblerInstructionNext(void *base, size_t *next);
int ps4KernelAssemblerInstructionSeek(void *base, size_t *offset);

// patch and hook

void ps4KernelFunctionPatchToJump(void *function, void *to);
void ps4KernelFunctionPatchToReturn(void *function, int64_t ret);

typedef struct Ps4KernelFunctionHook Ps4KernelFunctionHook;

//int ps4KernelFunctionHookCreate(Ps4KernFunctionHook **hookh, void *function, void *hook);
int ps4KernelFunctionHookCreateSized(Ps4KernelFunctionHook **hookh, void *function, void *hook, size_t size);
int ps4KernelFunctionHookDestroy(Ps4KernelFunctionHook *hookh);
int ps4KernelFunctionHookAdd(Ps4KernelFunctionHook *hookh, void *hook);
int ps4KernelFunctionHookRemove(Ps4KernelFunctionHook *hookh, void *hook);

//int ps4KernelFunctionHook(void *function, void *hook, Ps4KernFunctionHook **hookh);
int ps4KernelFunctionHookSized(void *function, void *hook, size_t size);
int ps4KernelFunctionIsHooked(void *function);
int ps4KernelFunctionGetHook(void *function, Ps4KernelFunctionHook **hook);
int ps4KernelFunctionUnhook(void *function);
