#define _KERNEL

#define _XOPEN_SOURCE 700
#define __BSD_VISIBLE 1
#define _WANT_UCRED
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/libkern.h>
#include <sys/malloc.h>
#include <sys/ucred.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <sys/proc.h>
#include <sys/kthread.h>
#include <sched.h>

#include <ps4/kern.h>
#include <ps4/payload.h>
#include <ps4/exception.h>
#include <ps4/base.h>

// Sonys version of the truth is 0 (most of the time ...)
void ps4KernFunctionPatchToReturn(void *function, register_t ret)
{
	ps4PayloadStubUtilReturnPatch(function, ret);
}

void ps4KernFunctionPatchToJumpUnsafe(void *function, void *to)
{
	ps4PayloadStubUtilJumpPatch(function, (register_t)to);
}

int ps4KernFunctionPatchToJump(void *function, void *to)
{
	if(ps4KernIsKernelAddress(function) != ps4KernIsKernelAddress(to))
		return PS4_ERROR_ARGUMENT_IN_DIFFERENT_SPACE; // no space shifting, el psy kongroo
	ps4KernFunctionPatchToJumpUnsafe(function, to);
	return PS4_OK;
}

int ps4KernFunctionIsJumpPatched(void *function)
{
	return ps4PayloadStubUtilJumpIs(function);
}

int ps4KernFunctionIsHooked(void *function)
{
	register_t value;
	if(ps4PayloadStubUtilJumpGet(function, &value) != PS4_OK)
		return PS4_NOT_OK;
	return ps4PayloadStubUtilJumpHookStage0Is((void *)value);
}

Ps4ExceptionArgument(ps4KernFunctionHookCreate,
	struct malloc_type *mt; struct mtx *mutex;
	Ps4KernFunctionHookArgument **argument);

Ps4ExceptionCatch(ps4KernFunctionHookCreate)
{
	Ps4KernFunctionHookArgument *arg = *e.argument;

	ps4FunctionCallIfNotNull(ps4KernMemoryFree, arg->stage0);
	ps4FunctionCallIfNotNull(ps4KernMemoryFree, arg->stage1);
	ps4FunctionCallIfNotNull(ps4KernMemoryFree, arg->bridge);
	ps4FunctionCallIfPrimaryNotNull(free, arg->hook, e.mt);
	ps4FunctionCallIfPrimaryNotNull(free, arg->arguments, e.mt);
	ps4FunctionCallIfPrimaryNotNull(free, arg, e.mt);
}

/*
ps4ExceptionFinally(ps4KernFunctionHookCreate)
{
	ps4FunctionCallIfNotNull(mtx_unlock, e.mutex);
}
*/

int ps4KernFunctionHookCreateSized(Ps4KernFunctionHook **hookh, void *function, void *hook, size_t size)
{
	Ps4KernFunctionHookArgument *arg = NULL;
	size_t jumpToFunctionSize = 0;
	size_t jumpToStage0Size = 0;
	struct malloc_type *mt = ps4KernDlSym("M_TEMP");
	struct mtx *mutex = ps4KernDlSym("Giant");

	Ps4ExceptionPlainTryCatch(ps4KernFunctionHookCreate, mt, mutex, &arg);

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, mt == NULL || mutex == NULL || function == NULL || hook == NULL);
	ps4ExceptionLeaveIf(PS4_ERROR_NOT_ACCEPTABLE, ps4KernFunctionIsHooked(function) == PS4_OK);

	arg = malloc(sizeof(Ps4KernFunctionHookArgument), mt, M_ZERO | M_WAITOK);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg == NULL);
	memset(arg, 0, sizeof(Ps4KernFunctionHookArgument));

	arg->arguments = malloc(sizeof(Ps4AssemblerRegisterParameter), mt, M_ZERO | M_WAITOK);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg->arguments == NULL);

	arg->hookSize = 8;
	arg->hook = malloc(arg->hookSize * sizeof(void *), mt, M_ZERO | M_WAITOK);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg->hook == NULL);

	ps4KernMemoryAllocate(&arg->stage0, ps4PayloadStubJumpHookStage0Size);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg->stage0 == NULL);

	// FIXME: Do not create stage 1 over and over - use only one
	ps4KernMemoryAllocate(&arg->stage1, ps4PayloadJumpHookStage1Size);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg->stage1 == NULL);

	// FIXME: Fix llvm for kernel -> remove size dependency
	jumpToStage0Size = 0;
	ps4PayloadStubUtilJumpSize((register_t)arg->stage0, &jumpToStage0Size);
	ps4ExceptionThrowIf(PS4_ERROR_ARGUMENT_SIZE_INVALID, size < jumpToStage0Size);
	jumpToStage0Size = size;
	//ps4PayloadStubUtilJumpSize((register_t)stage0, &jumpToStage0Size);
	//ps4MachineInstructionSeek(function, &jumpToStage0Size);
	ps4PayloadStubUtilJumpSize((register_t)function, &jumpToFunctionSize);

	arg->bridgeCopiedSize = jumpToStage0Size;
	ps4KernMemoryAllocate(&arg->bridge, arg->bridgeCopiedSize + jumpToFunctionSize);
	ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, arg->bridge == NULL);

	arg->function = function;
	memset(arg->hook, 0, arg->hookSize * sizeof(void *));
	arg->hook[0] = hook;
	arg->hookCount = 1;

	ps4PayloadStubUtilJumpHookStage0Patch(arg->stage0, (register_t)arg);
	ps4KernMemoryCopy((void *)ps4PayloadJumpHookStage1, arg->stage1, ps4PayloadJumpHookStage1Size);
	ps4KernMemoryCopy(arg->function, arg->bridge, arg->bridgeCopiedSize);

	ps4KernFunctionPatchToJump((void *)((uint8_t *)arg->bridge + arg->bridgeCopiedSize), (uint8_t *)arg->function + arg->bridgeCopiedSize);
	ps4KernFunctionPatchToJump(arg->function, arg->stage0);
	//ps4KernFunctionPatchToJump(arg->function, arg->bridge);

	if(hookh != NULL)
		*hookh = (Ps4KernFunctionHook *)arg;

	ps4ExceptionSuccess();
}

int ps4KernFunctionHookAdd(Ps4KernFunctionHook *hookh, void *hook)
{
	Ps4KernFunctionHookArgument *arg = (Ps4KernFunctionHookArgument *)hookh;
	struct malloc_type *mt = ps4KernDlSym("M_TEMP");
	//struct mtx *mutex = ps4KernDlSym("Giant");

	Ps4ExceptionPlainTry();

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);
	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, hook == NULL);

	if(arg->hookCount >= arg->hookSize - 1) // trailing NULL
	{
		void **m = malloc(2 * arg->hookSize * sizeof(void *), mt, M_ZERO | M_WAITOK);
		ps4ExceptionThrowIf(PS4_KERN_ERROR_OUT_OF_MEMORY, m == NULL);
		for(size_t t = 0; t < arg->hookCount; ++t)
			m[t] = arg->hook[t];
		free(arg->hook, mt);
		arg->hook = m;
		arg->hookSize *= 2;
	}

	arg->hook[arg->hookCount++] = hook;
	arg->hook[arg->hookCount] = NULL;

	ps4ExceptionSuccess();
}

int ps4KernFunctionHookRemove(Ps4KernFunctionHook *hookh, void *hook)
{
	Ps4KernFunctionHookArgument *arg = (Ps4KernFunctionHookArgument *)hookh;
	size_t s = 0;
	size_t t = 0;
	//struct mtx *mutex = ps4KernDlSym("Giant");

	Ps4ExceptionPlainTry();

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);
	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, hook == NULL);

	for(s = 0; arg->hook[s] != NULL;  ++s)
	{
		if(arg->hook[s] == hook)
			++t;
		if(t != 0)
			arg->hook[s] = arg->hook[s + t];
	}

	arg->hookCount -= t;

	ps4ExceptionFailIf(PS4_ERROR_NOT_FOUND, t == 0);
	ps4ExceptionSuccess();
}

int ps4KernFunctionHookSized(void *function, void *hook, size_t size)
{
	Ps4KernFunctionHook *hookh;
	if(ps4KernFunctionIsHooked(function) != PS4_OK)
 		return ps4KernFunctionHookCreateSized(&hookh, function, hook, size);
	ps4KernFunctionGetHook(function, &hookh);
	return ps4KernFunctionHookAdd(hookh, hook);
}

int ps4KernFunctionGetHook(void *function, Ps4KernFunctionHook **hook)
{
	register_t value;
	if(ps4PayloadStubUtilJumpGet(function, &value) != PS4_OK)
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
	return ps4PayloadStubUtilJumpHookStage0Get((void *)value, (register_t *)hook);
}

int ps4KernFunctionUnhook(void *function)
{
	Ps4KernFunctionHook *hook = NULL;
	if(ps4KernFunctionGetHook(function, &hook) != PS4_OK)
		return PS4_ERROR_EXPECTATION_FAILED;
	return ps4KernFunctionHookDestroy(hook);
}

int ps4KernFunctionHookDestroy(Ps4KernFunctionHook *hook)
{
	Ps4KernFunctionHookArgument *arg = (Ps4KernFunctionHookArgument *)hook;
	struct malloc_type *mt = ps4KernDlSym("M_TEMP");
	struct mtx *mutex = ps4KernDlSym("Giant");

	Ps4ExceptionPlainTryCatch(ps4KernFunctionHookCreate, mt, mutex, &arg);

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hook == NULL);

	// FIXME: make more robust / atomic lock loop / multiple back jumps, check lock
	ps4KernMemoryCopy(arg->bridge, arg->function, arg->bridgeCopiedSize);
	// ...

	ps4ExceptionThrow(PS4_OK); // let catch clean up, return OK
}
