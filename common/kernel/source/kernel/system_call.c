#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/sched.h>

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <bsm/audit_kevents.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <ps4/kernel.h>
#include <ps4/payload.h>
#include <ps4/exception.h>
#include <ps4/expression.h>
#include <ps4/base.h>
#include <ps4/atomic.h>
#include <ps4/machine.h>

#include <ps4/kernel.h>

#define PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK "ps4.kernel.system_call.hook"
#define PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_MUTEX PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK ".mutex"
#define PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_HANDLER PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK ".handler"
#define PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_CLONE PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK ".clone"

static struct mtx *ps4KernelSystemCallHookMutex;
static sy_call_t *ps4KernelSystemCallHookHandler;
static void *ps4KernelSystemCallHookArgumentClone;

int ps4KernelSystemCallHookInitialize()
{
	struct mtx *giant;
	int r;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("Giant", (void **)&giant));

	mtx_lock(giant);
	if(ps4KernelSystemCallHookMutex == NULL)
	{
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_MUTEX, (void *)&ps4KernelSystemCallHookMutex);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocateData((void **)&ps4KernelSystemCallHookMutex, sizeof(struct mtx));
			mtx_init(ps4KernelSystemCallHookMutex, "ps4KernelSystemCallHookMutex", NULL, MTX_DEF | MTX_RECURSE);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_MUTEX, ps4KernelSystemCallHookMutex);
		}
	}

	if(ps4KernelSystemCallHookHandler == NULL)
	{
		int ps4PayloadSystemCallHookHandlerSize = 512;
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_HANDLER, (void *)&ps4KernelSystemCallHookHandler);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocate((void **)&ps4KernelSystemCallHookHandler, ps4PayloadSystemCallHookHandlerSize);
			ps4KernelMemoryCopy((void *)ps4PayloadSystemCallHookHandler, (void *)ps4KernelSystemCallHookHandler, ps4PayloadSystemCallHookHandlerSize);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_HANDLER, (void *)ps4KernelSystemCallHookHandler);
		}
	}

	if(ps4KernelSystemCallHookArgumentClone == NULL)
	{
		int ps4PayloadSystemCallHookArgumentCloneSize = 768;
		r = ps4KernelCacheGlobalGet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_CLONE, (void *)&ps4KernelSystemCallHookArgumentClone);
		if(r != PS4_OK)
		{
			ps4KernelMemoryAllocate((void **)&ps4KernelSystemCallHookArgumentClone, ps4PayloadSystemCallHookArgumentCloneSize);
			ps4KernelMemoryCopy((void *)ps4PayloadSystemCallHookArgumentClone, (void *)ps4KernelSystemCallHookArgumentClone, ps4PayloadSystemCallHookArgumentCloneSize);
			ps4KernelCacheGlobalSet(PS4_KERNEL_CACHE_SYSTEM_CALL_HOOK_CLONE, ps4KernelSystemCallHookArgumentClone);
		}
	}
	mtx_unlock(giant);

	return PS4_OK;
}

Ps4ExceptionArgument(ps4KernelSystemCallHookExceptionHandler,
	Ps4KernelSystemCallHookArgument **argument);

Ps4ExceptionCatch(ps4KernelSystemCallHookExceptionHandler)
{
	Ps4KernelSystemCallHookArgument *arg = *e.argument;
	if(arg == NULL)
		return;

	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->hook);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg->hookType);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, (void *)arg->prologue);
	ps4FunctionCallIfNotNull(ps4KernelMemoryFree, arg);
}

Ps4ExceptionFinally(ps4KernelSystemCallHookExceptionHandler)
{
	ps4FunctionCallIfNotNull(mtx_unlock, ps4KernelSystemCallHookMutex);
}

int ps4KernelSystemCallCopyInAndPatch(int number, sy_call_t *call, size_t size, int argumentCount)//, uint32_t thrcnt)
{
	void *m;
	if(ps4KernelMemoryAllocate(&m, size) != PS4_OK)
		return PS4_ERROR_KERNEL_OUT_OF_MEMORY;
	ps4KernelMemoryCopy((void *)call, m, size);
	return ps4KernelSystemCallPatch(number, (sy_call_t *)m, argumentCount);
}

int ps4KernelSystemCallPatch(int number, sy_call_t *call, int argumentCount)//, uint32_t thrcnt)
{
	if(ps4KernelIsKernelAddress((void *)call) != PS4_OK)
		return PS4_ERROR_ARGUMENT_IN_DIFFERENT_SPACE; // do not hook kernel code to userland Oo
	if(number > SYS_MAXSYSCALL)
		return PS4_ERROR_KERNEL_SYSTEM_CALL_NUMBER_TOO_LARGE;
	return ps4KernelSystemCallPatchUnsafe(number, call, argumentCount);
}

int ps4KernelSystemCallPatchUnsafe(int number, sy_call_t *call, int argumentCount)
{
    struct sysent *sy;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("sysent", (void **)&sy));

	sched_pin();
    sy = &sy[number];
	memset(sy, 0, sizeof(struct sysent));
    sy->sy_narg = argumentCount;
    sy->sy_call = (sy_call_t *)call;
	sy->sy_auevent = AUE_NULL;
	sy->sy_thrcnt = SY_THR_STATIC;
	sched_unpin();

	return PS4_OK;
}

int ps4KernelSystemCallHookCreate(Ps4KernelSystemCallHook **hookh, int number)
{
	Ps4KernelSystemCallHookArgument *arg;
    struct sysent *sy;
	size_t prologueSize;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("sysent", (void **)&sy));

	if(hookh == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(number > SYS_MAXSYSCALL)
		return PS4_ERROR_KERNEL_SYSTEM_CALL_NUMBER_TOO_LARGE;

	if(ps4KernelSystemCallHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelSystemCallHookInitialize());

	mtx_lock(ps4KernelSystemCallHookMutex);

	ps4KernelMemoryAllocateData((void **)&arg, sizeof(Ps4KernelSystemCallHookArgument));
	if(arg == NULL)
		return PS4_ERROR_OUT_OF_MEMORY;
	ps4KernelMemoryFill(arg, 0, sizeof(Ps4KernelSystemCallHookArgument));

	arg->systemCalls = sy;
	arg->number = number;
	//arg->parent = arg;

	arg->hookSize = 8;
	arg->hookCount = 0;
	// FIXME: Either check or commit to WAIT semantic
	ps4KernelMemoryAllocateData((void **)&arg->hook, arg->hookSize * sizeof(sy_call_t *));
	ps4KernelMemoryAllocateData((void **)&arg->hookType, arg->hookSize * sizeof(int64_t));

	ps4KernelSymbolLookup("malloc", (void **)&arg->allocate);
	ps4KernelSymbolLookup("free", (void **)&arg->free);
	ps4KernelSymbolLookup("M_TEMP", (void **)&arg->mt);
	//ps4KernelSymbolLookup("_mtx_lock_sleep", (void **)&arg->lock);
	//ps4KernelSymbolLookup("_mtx_unlock_sleep", (void **)&arg->unlock);
	//ps4KernelSymbolLookup("atomic_cmpset_rel_ptr", (void **)&arg->release);
	//ps4KernelSymbolLookup("atomic_cmpset_acq_ptr", (void **)&arg->acquire);

	arg->clone = ps4KernelSystemCallHookArgumentClone;
	arg->handler = ps4KernelSystemCallHookHandler;
	//arg->mutex = ps4KernelSystemCallHookMutex;

	ps4PayloadSystemCallHookPrologueDeterminePatchSize(&prologueSize);
	ps4KernelMemoryAllocate((void **)&arg->prologue, prologueSize);
	ps4PayloadSystemCallHookProloguePatch((uint8_t *)arg->prologue, arg);

	sy = &sy[number];
	//arg->originalCall = arg->prologue;
	//ps4AtomicSwap64((uint64_t *)&sy->sy_call, (uint64_t *)&arg->originalCall);
	arg->originalCall = sy->sy_call;
	sy->sy_call = arg->prologue;

	*hookh = (Ps4KernelSystemCallHook *)arg;

	mtx_unlock(ps4KernelSystemCallHookMutex);

	return PS4_OK;
}

int ps4KernelSystemCallHookDestroy(Ps4KernelSystemCallHook *hookh)
{
	Ps4KernelSystemCallHookArgument *arg = (Ps4KernelSystemCallHookArgument *)hookh;
    struct sysent *sy;

	Ps4ExceptionTryCatchFinally(ps4KernelSystemCallHookExceptionHandler, &arg)

	if(ps4KernelSystemCallHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelSystemCallHookInitialize());

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("sysent", (void **)&sy));

	mtx_lock(ps4KernelSystemCallHookMutex);

	sy = &sy[arg->number];
	//ps4AtomicSwap64((uint64_t *)&sy->sy_call, (uint64_t *)&arg->originalCall);
	sy->sy_call = arg->originalCall;

	pause("Waiting for entered hooks to complete the prologue", 100);
	ps4AtomicSpinLock64(&arg->lock);
	while(arg->entryCount > 0)
	{
		ps4AtomicSpinUnlock64(&arg->lock);
		pause("Waiting for entered hooks to complete the prologue", 100);
		ps4AtomicSpinLock64(&arg->lock);
	}
	ps4AtomicSpinUnlock64(&arg->lock);

	ps4ExceptionThrow(PS4_OK);
}

int ps4KernelSystemCallHookAdd(Ps4KernelSystemCallHook *hookh, void *hook, Ps4KernelSystemCallHookType type)
{
	Ps4KernelSystemCallHookArgument *arg = (Ps4KernelSystemCallHookArgument *)hookh;

	Ps4ExceptionTryFinally(ps4KernelSystemCallHookExceptionHandler, &arg);

	if(ps4KernelSystemCallHookMutex == NULL)
		ps4ExpressionReturnOnError(ps4KernelSystemCallHookInitialize());

	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_PRIMARY_MISSING, hookh == NULL);
	ps4ExceptionLeaveIf(PS4_ERROR_ARGUMENT_MISSING, hook == NULL);

	mtx_lock(ps4KernelSystemCallHookMutex);

	if(arg->hookCount >= arg->hookSize)
	{
		sy_call_t **m;
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

	arg->hook[arg->hookCount] = (sy_call_t *)hook;
	arg->hookType[arg->hookCount] = (int64_t)type;
	++arg->hookCount;

	ps4ExceptionSuccess();
}
