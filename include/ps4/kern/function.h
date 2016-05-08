#pragma once

#include <ps4/assembler.h>

#define PS4_KERN_FUNCTION_HOOK_BREAK 1
#define PS4_KERN_FUNCTION_HOOK_CONTINUE 0
#define PS4_KERN_FUNCTION_HOOK_RETURN -1

#define PS4_KERN_FUNCTION_HOOK_PERFORM_CALL PS4_KERN_FUNCTION_HOOK_BREAK
#define PS4_KERN_FUNCTION_HOOK_NEXT_HOOK PS4_KERN_FUNCTION_HOOK_CONTINUE
#define PS4_KERN_FUNCTION_HOOK_INTERCEPT_CALL PS4_KERN_FUNCTION_HOOK_RETURN

typedef struct Ps4KernFunctionHookArgument // shared with asm, watch offsets
{
	void *function; // 0
	void **hook; // asm use
	int64_t hookCurrent; // 16 // asm temp use
	int64_t hookCount;
	int64_t hookSize; // 32
	void *bridge; // asm use
	int64_t bridgeCopiedSize; // 48
	Ps4AssemblerRegisterParameter *arguments; // asm use
	int64_t locked; // 64 // asm use
	void *reserved1;
	int64_t entryCount; // 80 // asm use
	int64_t callCount; // asm use
	void *returnTo; //96 // asm temp use
	int64_t r12; // asm temp use
	void *stage1; //112 // asm use
	void *stage0; // asm use
}
Ps4KernFunctionHookArgument;

typedef struct Ps4KernFunctionHook Ps4KernFunctionHook;

void ps4KernFunctionPatchToReturn(void *function, register_t ret);
int ps4KernFunctionPatchToJump(void *function, void *to);
void ps4KernFunctionPatchToJumpUnsafe(void *function, void *to);

//int ps4KernFunctionHookCreate(Ps4KernFunctionHook **hookh, void *function, void *hook);
int ps4KernFunctionHookCreateSized(Ps4KernFunctionHook **hookh, void *function, void *hook, size_t size);
int ps4KernFunctionHookDestroy(Ps4KernFunctionHook *hookh);
int ps4KernFunctionHookAdd(Ps4KernFunctionHook *hookh, void *hook);
int ps4KernFunctionHookRemove(Ps4KernFunctionHook *hookh, void *hook);

//int ps4KernFunctionHook(void *function, void *hook, Ps4KernFunctionHook **hookh);
int ps4KernFunctionHookSized(void *function, void *hook, size_t size);
int ps4KernFunctionIsHooked(void *function);
int ps4KernFunctionGetHook(void *function, Ps4KernFunctionHook **hook);
int ps4KernFunctionUnhook(void *function);
