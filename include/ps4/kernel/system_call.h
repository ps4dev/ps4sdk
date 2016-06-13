#pragma once

#undef offsetof
#include <sys/sysent.h>

typedef enum Ps4KernelSystemCallHookType
{
	PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_PRE = -1,
	PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_DISABLED = 0,
	PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_PRE = 1,
	PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_POST = 2,
	PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_BOTH = 3
}
Ps4KernelSystemCallHookType;

typedef enum Ps4KernelSystemCallHookControl
{
	PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_BREAK = 1,
	PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_CONTINUE = 0,
	PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_RETURN = -1,
}
Ps4KernelSystemCallHookControl;

#define PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_PERFORM_CALL PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_BREAK
#define PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_NEXT_HOOK PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_CONTINUE
#define PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_INTERCEPT_CALL PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_RETURN

typedef void *Ps4SystemCallHookAllocate(size_t, void *, int);
typedef void Ps4SystemCallHookFree(void *, void *);

typedef struct Ps4KernelSystemCallHookArgument
{
	// asm begin
	void *clone;
	sy_call_t *handler;
	void *thread;
	int64_t entryCount;
	int64_t callCount;
	uint64_t lock;
	// asm end

	sy_call_t *prologue;

	void *uap;
	int sysret;
	register_t returns[2];
	sy_call_t *originalCall;
	struct sysent *systemCalls;
	int64_t number;

	sy_call_t **hook;
	int64_t *hookType;
	int64_t hookCount;
	int64_t hookSize;
	int64_t hookTypeCurrent;

	void *userArgument;

	Ps4SystemCallHookAllocate *allocate;
	Ps4SystemCallHookFree *free;
	void *mt;
}
Ps4KernelSystemCallHookArgument;

typedef struct Ps4KernelSystemCallHook Ps4KernelSystemCallHook;

int ps4KernelSystemCallPatch(int number, sy_call_t *call, int argumentCount);
int ps4KernelSystemCallPatchUnsafe(int number, sy_call_t *call, int argumentCount);
int ps4KernelSystemCallCopyInAndPatch(int number, sy_call_t *call, size_t size, int argumentCount);

int ps4KernelSystemCallHook(int number, sy_call_t *hook);
int ps4KernelSystemCallUnhook(int number);

int ps4KernelSystemCallHookCreate(Ps4KernelSystemCallHook **hookh, int number);
int ps4KernelSystemCallHookDestroy(Ps4KernelSystemCallHook *hookh);
int ps4KernelSystemCallHookAdd(Ps4KernelSystemCallHook *hookh, void *hook, Ps4KernelSystemCallHookType type);
