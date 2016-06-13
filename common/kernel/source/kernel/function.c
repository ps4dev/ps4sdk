#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <sys/proc.h>
#include <sched.h>

#include <ps4/kernel.h>
#include <ps4/payload.h>
#include <ps4/exception.h>
#include <ps4/expression.h>
#include <ps4/base.h>
#include <ps4/atomic.h>
#include <ps4/machine.h>

#define PS4_KERNEL_CACHE_FUNCTION_HOOK "ps4.kernel.function.hook"
#define PS4_KERNEL_CACHE_FUNCTION_HOOK_MUTEX PS4_KERNEL_CACHE_FUNCTION_HOOK ".mutex"
#define PS4_KERNEL_CACHE_FUNCTION_HOOK_HANDLER PS4_KERNEL_CACHE_FUNCTION_HOOK ".handler"
#define PS4_KERNEL_CACHE_FUNCTION_HOOK_CLONE PS4_KERNEL_CACHE_FUNCTION_HOOK ".clone"
#define PS4_KERNEL_CACHE_FUNCTION_HOOK_DESTROY PS4_KERNEL_CACHE_FUNCTION_HOOK ".destroy"

static struct mtx *ps4KernelFunctionHookMutex;
static void *ps4KernelFunctionHookHandler;
static void *ps4FunctionHookArgumentClone;
static void *ps4FunctionHookArgumentDestroy;

int ps4KernelFunctionHookInitialize()
{
	struct mtx *giant;
	int r;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("Giant", (void **)&giant));

	mtx_lock(giant);

	if(ps4KernelFunctionHookMutex == NULL)
	{
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_FUNCTION_HOOK_MUTEX, (void *)&ps4KernelFunctionHookMutex);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocateData((void **)&ps4KernelFunctionHookMutex, sizeof(struct mtx));
			mtx_init(ps4KernelFunctionHookMutex, "ps4KernelFunctionHookMutex", NULL, MTX_DEF | MTX_RECURSE);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_FUNCTION_HOOK_MUTEX, ps4KernelFunctionHookMutex);
		}
	}

	if(ps4KernelFunctionHookHandler == NULL)
	{
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_FUNCTION_HOOK_HANDLER, (void *)&ps4KernelFunctionHookHandler);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocate(&ps4KernelFunctionHookHandler, ps4PayloadFunctionHookHandlerSize);
			ps4KernelMemoryCopy((void *)ps4PayloadFunctionHookHandler, ps4KernelFunctionHookHandler, ps4PayloadFunctionHookHandlerSize);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_FUNCTION_HOOK_HANDLER, ps4KernelFunctionHookHandler);
		}
	}

	if(ps4FunctionHookArgumentClone == NULL)
	{
		int ps4FunctionHookArgumentCloneSize = 896;
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_FUNCTION_HOOK_CLONE, (void *)&ps4FunctionHookArgumentClone);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocate(&ps4FunctionHookArgumentClone, ps4FunctionHookArgumentCloneSize);
			ps4KernelMemoryCopy((void *)ps4PayloadFunctionHookArgumentClone, ps4FunctionHookArgumentClone, ps4FunctionHookArgumentCloneSize);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_FUNCTION_HOOK_CLONE, ps4FunctionHookArgumentClone);
		}
	}

	if(ps4FunctionHookArgumentDestroy == NULL)
	{
		int ps4FunctionHookArgumentDestroySize = 128;
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_FUNCTION_HOOK_DESTROY, (void *)&ps4FunctionHookArgumentDestroy);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocate(&ps4FunctionHookArgumentDestroy, ps4FunctionHookArgumentDestroySize);
			ps4KernelMemoryCopy((void *)ps4PayloadFunctionHookArgumentDestroy, ps4FunctionHookArgumentDestroy, ps4FunctionHookArgumentDestroySize);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_FUNCTION_HOOK_DESTROY, ps4FunctionHookArgumentDestroy);
		}
	}

	mtx_unlock(giant);

	return PS4_OK;
}

Ps4ExceptionArgument(ps4KernelFunctionHookExceptionHandler,
	Ps4KernelFunctionHookArgument **argument);

Ps4ExceptionCatch(ps4KernelFunctionHookExceptionHandler)
{
	Ps4KernelFunctionHookArgument *arg = *e.argument;
	if(arg == NULL)
		return;

	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->prologue);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->bridge);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->hook);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->hookType);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->arguments);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->returns);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg);
}

Ps4ExceptionFinally(ps4KernelFunctionHookExceptionHandler)
{
	ps4FunctionCallIfNotNull(mtx_unlock, ps4KernelFunctionHookMutex);
}

void ps4KernelFunctionPatchToReturn(void *function, register_t ret)
{
	ps4PayloadReturnPatch(function, ret);
}

int ps4KernelFunctionIsPatchedToReturn(void *function)
{
	return ps4PayloadReturnIsPatched(function);
}

void ps4KernelFunctionPatchToJumpUnsafe(void *function, void *to)
{
	ps4PayloadJumpPatch(function, to);
}

int ps4KernelFunctionPatchToJump(void *function, void *to)
{
	if(ps4KernelIsKernelAddress(function) != ps4KernelIsKernelAddress(to))
		return PS4_ERROR_ARGUMENT_IN_DIFFERENT_SPACE; // no space shifting, el psy kongroo
	ps4KernelFunctionPatchToJumpUnsafe(function, to);
	return PS4_OK;
}

int ps4KernelFunctionIsPatchedToJump(void *function)
{
	return ps4PayloadJumpIsPatched(function);
}

int ps4KernelFunctionIsHooked(void *function)
{
	void *adr;
	if(ps4PayloadJumpExtractTarget(function, &adr) != PS4_OK)
		return PS4_NOT_OK;
	return ps4PayloadFunctionHookPrologueIsPatched(adr);
}

int ps4KernelFunctionLock(void *function, uint16_t *bits)
{
	if(function == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(bits == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	sched_pin();
	ps4KernelMemoryCopy((void *)ps4PayloadLock, (void *)bits, ps4PayloadLockSize);
	ps4KernelProtectionWriteDisable();
	ps4AtomicSwap16(function, bits);
	ps4KernelProtectionWriteEnable();
	sched_unpin();

	return PS4_OK;
}

int ps4KernelFunctionUnlock(void *function, uint16_t bits)
{
	if(function == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	sched_pin();
	ps4KernelProtectionWriteDisable();
	ps4AtomicSwap16(function, &bits);
	ps4KernelProtectionWriteEnable();
	sched_unpin();

	return PS4_OK;
}

int ps4KernelFunctionHookSetUserArgument(Ps4KernelFunctionHook *hookh, void *argument)
{
	Ps4KernelFunctionHookArgument *arg = (Ps4KernelFunctionHookArgument *)hookh;

	if(hookh == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	arg->userArgument = argument;

	return PS4_OK;
}

int ps4KernelFunctionHookGetUserArgument(Ps4KernelFunctionHook *hookh, void **argument)
{
	Ps4KernelFunctionHookArgument *arg = (Ps4KernelFunctionHookArgument *)hookh;

	if(hookh == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(argument == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*argument = arg->userArgument;

	return PS4_OK;
}

int ps4KernelFunctionHookCreate(Ps4KernelFunctionHook **hookh, void *function)
{
	Ps4KernelFunctionHookArgument *arg = NULL;
	size_t jumpToFunctionSize = 0;
	size_t jumpToFunctionHookPrologueSize = 0;
	size_t prologueSize;
	void *t;

	Ps4ExceptionTryCatchFinally(ps4KernelFunctionHookExceptionHandler, &arg);

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, function == NULL);

	mtx_lock(ps4KernelFunctionHookMutex);

	ps4ExceptionFailIf(PS4_ERROR_NOT_ACCEPTABLE, ps4KernelFunctionIsHooked(function) == PS4_OK);

	ps4KernelMemoryAllocateData((void *)&arg, sizeof(Ps4KernelFunctionHookArgument));
	ps4ExceptionFailIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg == NULL);
	ps4KernelMemoryFill(arg, 0, sizeof(Ps4KernelFunctionHookArgument));

	ps4KernelMemoryAllocateData((void *)&arg->arguments, sizeof(Ps4RegisterParameters));
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->arguments == NULL);
	ps4KernelMemoryFill(arg->arguments, 0, sizeof(Ps4RegisterParameters));

	ps4KernelMemoryAllocateData((void *)&arg->returns, sizeof(Ps4RegisterReturns));
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->returns == NULL);
	ps4KernelMemoryFill(arg->returns, 0, sizeof(Ps4RegisterReturns));

	arg->hookSize = 8; // initial space for 7 hooks + NULL
	ps4KernelMemoryAllocateData((void *)&arg->hook, arg->hookSize * sizeof(void *));
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->hook == NULL);
	ps4KernelMemoryFill(arg->hook, 0, arg->hookSize * sizeof(void *));

	ps4KernelMemoryAllocateData((void *)&arg->hookType, arg->hookSize * sizeof(int64_t));
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->hookType == NULL);
	ps4KernelMemoryFill(arg->hookType, 0, arg->hookSize * sizeof(int64_t));

	ps4PayloadFunctionHookPrologueDeterminePatchSize(&prologueSize);
	ps4KernelMemoryAllocate(&arg->prologue, prologueSize);
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->prologue == NULL);

	// FIXME: Fix llvm for kernel -> remove size dependency
	ps4PayloadJumpDeterminePatchSize(arg->prologue, &jumpToFunctionHookPrologueSize);
	ps4MachineInstructionSeek(function, &t, jumpToFunctionHookPrologueSize);
	arg->bridgeCopiedSize = (uint8_t *)t - (uint8_t *)function;
	ps4PayloadJumpDeterminePatchSize(function, &jumpToFunctionSize);
	ps4KernelMemoryAllocate(&arg->bridge, arg->bridgeCopiedSize + jumpToFunctionSize);
	ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, arg->bridge == NULL);

	arg->function = function;
	//arg->hook[0] = hook;
	arg->hookCount = 0;

	arg->handler = ps4KernelFunctionHookHandler;
	//arg->mutex = ps4KernelFunctionHookMutex;
	arg->clone = (void *)ps4FunctionHookArgumentClone;
	arg->epilogue = (void *)ps4FunctionHookArgumentDestroy;

	ps4KernelSymbolLookup("malloc", (void **)&arg->allocate);
	ps4KernelSymbolLookup("free", (void **)&arg->free);
	ps4KernelSymbolLookup("M_TEMP", (void **)&arg->mt);
	//ps4KernelSymbolLookup("_mtx_lock_sleep", (void **)&arg->lock);
	//ps4KernelSymbolLookup("_mtx_unlock_sleep", (void **)&arg->unlock);

	ps4PayloadFunctionHookProloguePatch(arg->prologue, arg);
	ps4KernelMemoryCopy(arg->function, arg->bridge, arg->bridgeCopiedSize);

	ps4KernelFunctionPatchToJump((void *)((uint8_t *)arg->bridge + arg->bridgeCopiedSize), (uint8_t *)arg->function + arg->bridgeCopiedSize);
	ps4KernelFunctionPatchToJump(arg->function, arg->prologue);

	if(hookh != NULL)
		*hookh = (Ps4KernelFunctionHook *)arg;

	ps4ExceptionSuccess();
}

int ps4KernelFunctionHookAdd(Ps4KernelFunctionHook *hookh, void *hook, Ps4KernelFunctionHookType type)
{
	Ps4KernelFunctionHookArgument *arg = (Ps4KernelFunctionHookArgument *)hookh;

	Ps4ExceptionTryFinally(ps4KernelFunctionHookExceptionHandler, &arg);

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);
	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, hook == NULL);

	mtx_lock(ps4KernelFunctionHookMutex);

	ps4AtomicSpinLock64(&arg->lock);
	if(arg->hookCount >= arg->hookSize - 1) // trailing NULL
	{
		void **m;
		int64_t *m2;

		ps4KernelMemoryAllocateData((void *)&m, 2 * arg->hookSize * sizeof(void *));
		ps4ExceptionThrowIf(PS4_ERROR_KERNEL_OUT_OF_MEMORY, m == NULL);
		ps4KernelMemoryAllocateData((void *)&m2, 2 * arg->hookSize * sizeof(int64_t));
		if(m2 == NULL)
		{
			ps4KernelMemoryFree(m);
			ps4ExceptionThrow(PS4_ERROR_KERNEL_OUT_OF_MEMORY);
		}
		for(size_t t = 0; t < arg->hookCount; ++t)
		{
			m[t] = arg->hook[t];
			m2[t] = arg->hookType[t];
		}
		ps4KernelMemoryFree(arg->hook);
		ps4KernelMemoryFree(arg->hookType);
		arg->hook = m;
		arg->hookType = m2;
		arg->hookSize *= 2;
	}
	ps4AtomicSpinUnlock64(&arg->lock);

	arg->hook[arg->hookCount] = hook;
	arg->hookType[arg->hookCount] = (int64_t)type;
	++arg->hookCount;
	arg->hook[arg->hookCount] = NULL;

	ps4ExceptionSuccess();
}

int ps4KernelFunctionHookRemove(Ps4KernelFunctionHook *hookh, void *hook)
{
	Ps4KernelFunctionHookArgument *arg = (Ps4KernelFunctionHookArgument *)hookh;
	size_t s = 0;
	size_t t = 0;

	ps4ExpressionReturnIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);
	ps4ExpressionReturnIf(PS4_ERROR_ARGUMENT_MISSING, hook == NULL);

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	mtx_lock(ps4KernelFunctionHookMutex);

	ps4AtomicSpinLock64(&arg->lock);
	for(s = 0; arg->hook[s] != NULL;  ++s)
	{
		if(arg->hook[s] == hook)
			++t;
		if(t != 0)
			arg->hook[s] = arg->hook[s + t];
	}
	arg->hookCount -= t;
	ps4AtomicSpinUnlock64(&arg->lock);

	mtx_unlock(ps4KernelFunctionHookMutex);

	ps4ExpressionReturnIf(PS4_ERROR_NOT_FOUND, t == 0);

	return PS4_OK;
}

int ps4KernelFunctionHookDestroy(Ps4KernelFunctionHook *hook)
{
	Ps4KernelFunctionHookArgument *arg = (Ps4KernelFunctionHookArgument *)hook;
	uint16_t fnBits;

	Ps4ExceptionTryCatchFinally(ps4KernelFunctionHookExceptionHandler, &arg);

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hook == NULL);

	mtx_lock(ps4KernelFunctionHookMutex);

	sched_pin();
	ps4KernelFunctionLock(arg->function, &fnBits);
	ps4KernelMemoryCopy((uint8_t *)arg->bridge + 2, (uint8_t *)arg->function + 2, arg->bridgeCopiedSize - 2);
	ps4KernelMemoryCopy(arg->bridge, &fnBits, 2);
	ps4KernelFunctionUnlock(arg->function, fnBits);
	sched_unpin();

	pause("Waiting for entered hooks to complete the prologue", 100);
	ps4AtomicSpinLock64(&arg->lock);
	while(arg->entryCount > 0)
	{
		ps4AtomicSpinUnlock64(&arg->lock);
		pause("Waiting for entered hooks to complete the prologue", 100);
		ps4AtomicSpinLock64(&arg->lock);
	}
	ps4AtomicSpinUnlock64(&arg->lock);

	ps4ExceptionThrow(PS4_OK); // let catch clean up, return OK
}

int ps4KernelFunctionHook(void *function, void *hook, Ps4KernelFunctionHookType type)
{
	Ps4KernelFunctionHook *hookh;
	int r;

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	mtx_lock(ps4KernelFunctionHookMutex);

	if(ps4KernelFunctionIsHooked(function) != PS4_OK)
 	{
		r = ps4KernelFunctionHookCreate(&hookh, function);
 		if(r == PS4_OK)
			r = ps4KernelFunctionHookAdd(hookh, hook, type);

		mtx_unlock(ps4KernelFunctionHookMutex);
		return r;
	}

	r = ps4KernelFunctionGetHook(function, &hookh);
	if(r == PS4_OK)
		r = ps4KernelFunctionHookAdd(hookh, hook, type);

	mtx_unlock(ps4KernelFunctionHookMutex);

	return r;
}

int ps4KernelFunctionPosthook(void *function, Ps4KernelFunctionHookGeneric *hook)
{
	return ps4KernelFunctionHook(function, (void *)hook, PS4_KERNEL_FUNCTION_HOOK_TYPE_GENERIC_POST);
}

int ps4KernelFunctionPrehook(void *function, void *hook)
{
	return ps4KernelFunctionHook(function, hook, PS4_KERNEL_FUNCTION_HOOK_TYPE_PRE);
}

int ps4KernelFunctionGetHook(void *function, Ps4KernelFunctionHook **hook)
{
	void *adr;
	int r;

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	mtx_lock(ps4KernelFunctionHookMutex);
	if(ps4PayloadJumpExtractTarget(function, &adr) != PS4_OK)
		r = PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
	else
		r = ps4PayloadFunctionHookPrologueExtractArgument(adr, (Ps4KernelFunctionHookArgument **)hook);
	mtx_unlock(ps4KernelFunctionHookMutex);

	return r;
}

int ps4KernelFunctionUnhook(void *function)
{
	Ps4KernelFunctionHook *hook = NULL;
	int r;

	if(ps4KernelFunctionHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelFunctionHookInitialize());

	mtx_lock(ps4KernelFunctionHookMutex);
	if(ps4KernelFunctionGetHook(function, &hook) != PS4_OK)
		r = PS4_ERROR_EXPECTATION_FAILED;
	else
		r = ps4KernelFunctionHookDestroy(hook);
	mtx_unlock(ps4KernelFunctionHookMutex);

	return r;
}
