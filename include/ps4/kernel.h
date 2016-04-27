#pragma once

#include <sys/sysent.h>
#include <stdint.h>

#include <ps4/default_argument.h>

// extension based kernel execution
int ps4KernelExecute(void *call, void *uap, int64_t *ret0, int64_t *ret1);

// Omnislash - 9999, 9999, .... - Mimic - ...
int64_t ps4KernelCall(void *call, int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9);

#define ps4KernelCall(...) \
	ps4KernelCall__(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0)

#define ps4KernelCall__(call, rdi, rsi, rdx, rcx, r8, r9, ...) \
	ps4KernelCall_((void *)(call), \
		(int64_t)Ps4DefaultArgument((rdi), int64_t, 0), \
		(int64_t)Ps4DefaultArgument((rsi), int64_t, 0), \
		(int64_t)Ps4DefaultArgument((rdx), int64_t, 0), \
		(int64_t)Ps4DefaultArgument((rcx), int64_t, 0), \
		(int64_t)Ps4DefaultArgument((r8), int64_t, 0), \
		(int64_t)Ps4DefaultArgument((r9), int64_t, 0) \
	)

// info
void *ps4KernelDlSym(char *name);

int ps4KernelIsInKernel();
int ps4KernelIsKernelAddress(void *addr);

// sec stuff
void ps4KernelUartEnable();
void ps4KernelUartDisable();

void ps4KernelPrivilegeRoot();
int ps4KernelPrivilegeUnjail();
int ps4KernelPrivilegeEscalate();

// util
void ps4KernelFunctionPatchToJumpTo(void *function, void *to);
void ps4KernelFunctionPatchToTruth(void *function);

// memory
int ps4KernelMemoryAllocate(void **memory, size_t size);
void *ps4KernelMemoryMalloc(size_t size);
void ps4KernelMemoryFree(void *addr);
void ps4KernelMemoryCopy(void *src, void *dest, size_t size);
