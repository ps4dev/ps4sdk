#pragma once

#include <ps4/register.h>

typedef enum Ps4KernelFunctionHookType
{
	PS4_KERNEL_FUNCTION_HOOK_TYPE_PRE = -1,
	PS4_KERNEL_FUNCTION_HOOK_TYPE_DISABLED = 0,
	PS4_KERNEL_FUNCTION_HOOK_TYPE_GENERIC_PRE = 1,
	PS4_KERNEL_FUNCTION_HOOK_TYPE_GENERIC_POST = 2,
	PS4_KERNEL_FUNCTION_HOOK_TYPE_GENERIC_BOTH = 3,
}
Ps4KernelFunctionHookType;

typedef enum Ps4KernelFunctionHookControl
{
	PS4_KERNEL_FUNCTION_HOOK_CONTROL_BREAK = 1,
	PS4_KERNEL_FUNCTION_HOOK_CONTROL_CONTINUE = 0,
	PS4_KERNEL_FUNCTION_HOOK_CONTROL_RETURN = -1,
}
Ps4KernelFunctionHookControl;

#define PS4_KERNEL_FUNCTION_HOOK_CONTROL_PERFORM_CALL PS4_KERNEL_FUNCTION_HOOK_CONTROL_BREAK
#define PS4_KERNEL_FUNCTION_HOOK_CONTROL_NEXT_HOOK PS4_KERNEL_FUNCTION_HOOK_CONTROL_CONTINUE
#define PS4_KERNEL_FUNCTION_HOOK_CONTROL_INTERCEPT_CALL PS4_KERNEL_FUNCTION_HOOK_CONTROL_RETURN

/*
typedef struct Ps4KernelFunctionHookArgument // shared with asm, watch offsets
{
	void *function; // 0
	void **hook; // asm use
	int64_t hookCurrent; // 16 // asm temp use
	int64_t hookCount;
	int64_t hookSize; // 32
	void *bridge; // asm use
	int64_t bridgeCopiedSize; // 48
	Ps4RegisterParameters *arguments; // asm use
	uint64_t lock; // 64 // asm use
	void *hookTypeCurrent; // 72
	int64_t entryCount; // 80 // asm use
	int64_t callCount; // asm use
	void *returnTo; //96 // asm temp use
	int64_t r12; // asm temp use
	void *handler; //112 // asm use
	void *prologue; // asm use
	int64_t *hookType; // 128 // asm use
	//int64_t returned[2]; // asm use 136, 144
	//Ps4UnsignedInteger128 returnedFloat[2]; // asm use 152
	Ps4RegisterReturns *returns; // 136 asm use
	void *userArgument;
}
Ps4KernelFunctionHookArgument;
*/
typedef void *Ps4SystemFunctionAllocate(size_t, void *, int);
typedef void Ps4SystemFunctionFree(void *, void *);

typedef struct Ps4KernelFunctionHookArgument // shared with asm, watch offsets
{
	// asm prologue begin
	void *clone;
	void *handler; // 8
	int64_t entryCount;
	void *returnTo; // 24
	int64_t r12;
	 // asm prologue end

 	void *prologue; // 40
 	void *epilogue;
 	int64_t callCount; // 56

	Ps4RegisterParameters *arguments;
	Ps4RegisterReturns *returns; // 72
	void *function;
	void *bridge; // 88
	int64_t bridgeCopiedSize;

	void **hook; // 104
	int64_t/*Ps4KernelFunctionHookType*/ *hookType;
	int64_t hookCurrent; // 120
	int64_t hookCount;
	int64_t hookSize; // 136
	int64_t hookTypeCurrent;

	void *userArgument; // 152

	Ps4SystemFunctionAllocate *allocate;
	Ps4SystemFunctionFree *free; // 168
	void *mt;

	uint64_t lock; // 184
}
Ps4KernelFunctionHookArgument;

typedef struct Ps4KernelFunctionHook Ps4KernelFunctionHook;

typedef int Ps4KernelFunctionHookGeneric(struct thread *td, Ps4KernelFunctionHookArgument *arg);


int ps4KernelFunctionLock(void *function, uint16_t *bits);
int ps4KernelFunctionUnlock(void *function, uint16_t bits);

void ps4KernelFunctionPatchToReturn(void *function, register_t ret);
int ps4KernelFunctionIsPatchToReturn(void *function);

void ps4KernelFunctionPatchToJumpUnsafe(void *function, void *to);
int ps4KernelFunctionPatchToJump(void *function, void *to);
int ps4KernelFunctionIsPatchedToJump(void *function);

int ps4KernelFunctionHookSetUserArgument(Ps4KernelFunctionHook *hookh, void *argument);
int ps4KernelFunctionHookGetUserArgument(Ps4KernelFunctionHook *hookh, void **argument);

int ps4KernelFunctionIsHooked(void *function);
int ps4KernelFunctionHookCreate(Ps4KernelFunctionHook **hookh, void *function);
int ps4KernelFunctionHookAdd(Ps4KernelFunctionHook *hookh, void *hook, Ps4KernelFunctionHookType type);
int ps4KernelFunctionHookRemove(Ps4KernelFunctionHook *hookh, void *hook);
int ps4KernelFunctionHookDestroy(Ps4KernelFunctionHook *hook);

int ps4KernelFunctionHook(void *function, void *hook, Ps4KernelFunctionHookType type);
int ps4KernelFunctionPrehook(void *function, void *hook);
int ps4KernelFunctionPosthook(void *function, Ps4KernelFunctionHookGeneric *hook);
int ps4KernelFunctionGetHook(void *function, Ps4KernelFunctionHook **hook);
int ps4KernelFunctionUnhook(void *function);
