#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>

#include <sys/sysctl.h>
#include <sys/malloc.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <ps4/kernel.h>

#include <ps4/atomic.h>

inline void ps4PayloadFunctionHookMemoryCopy(void *a, void *b, size_t s)
{
	uint8_t *old = (uint8_t *)a;
	uint8_t *new = (uint8_t *)b;
	for(size_t i = 0; i < s; ++i)
		new[i] = old[i];
}

Ps4KernelFunctionHookArgument *ps4PayloadFunctionHookArgumentClone(Ps4KernelFunctionHookArgument *uap)
{
	Ps4KernelFunctionHookArgument *uap2;

	//FIXME: should be lock and copyable
	//if(!uap->acquire(&(uap->mutex)->mtx_lock, MTX_UNOWNED, curthread))
	//	uap->lock(uap->mutex, curthread, 0, LOCK_FILE, LOCK_LINE);
	//mtx_lock(uap->mutex);

	// FIXME: Sleeping should be avoided
 	uap2 = uap->allocate(sizeof(Ps4KernelFunctionHookArgument), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadFunctionHookMemoryCopy(uap, uap2, sizeof(Ps4KernelFunctionHookArgument));

	uap2->hook = uap->allocate(uap->hookSize * sizeof(void *), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadFunctionHookMemoryCopy(uap->hook, uap2->hook, uap->hookSize * sizeof(void *));

	uap2->hookType = uap->allocate(uap->hookSize * sizeof(int64_t), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadFunctionHookMemoryCopy(uap->hookType, uap2->hookType, uap->hookSize * sizeof(int64_t));

	uap2->arguments = uap->allocate(sizeof(Ps4RegisterParameters), uap->mt, M_ZERO | M_WAITOK);
	//ps4PayloadFunctionHookMemoryCopy(uap->arguments, uap2->arguments, sizeof(Ps4RegisterParameters));

	uap2->returns = uap->allocate(sizeof(Ps4RegisterReturns), uap->mt, M_ZERO | M_WAITOK);
	//ps4PayloadFunctionHookMemoryCopy(uap->returns, uap2->returns, sizeof(Ps4RegisterReturns));

	//++uap->callCount;

	//if(!uap->release(&(uap->mutex)->mtx_lock, curthread, MTX_UNOWNED))
	//	uap->unlock(uap->mutex, curthread, 0, LOCK_FILE, LOCK_LINE);
	//mtx_unlock(uap->mutex);

	return uap2;
}

void ps4PayloadFunctionHookArgumentDestroy(Ps4KernelFunctionHookArgument *uap)
{
	uap->free(uap->returns, uap->mt);
	uap->free(uap->arguments, uap->mt);
	uap->free(uap->hookType, uap->mt);
	uap->free(uap->hook, uap->mt);
	uap->free(uap, uap->mt);
}
