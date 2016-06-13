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

inline void ps4PayloadSystemCallHookMemoryCopy(void *a, void *b, size_t s)
{
	uint8_t *old = (uint8_t *)a;
	uint8_t *new = (uint8_t *)b;
	for(size_t i = 0; i < s; ++i)
		new[i] = old[i];
}

static inline register_t ps4PayloadSystemCallHookThreadGetReturn0(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x380]));
}

static inline register_t ps4PayloadSystemCallHookThreadGetReturn1(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x388]));
}

static inline void ps4PayloadSystemCallHookThreadSetReturn0(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x380])) = ret;
}

static inline void ps4PayloadSystemCallHookThreadSetReturn1(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x388])) = ret;
}

Ps4KernelSystemCallHookArgument *ps4PayloadSystemCallHookArgumentClone(struct thread *td, Ps4KernelSystemCallHookArgument *uap, void *sysuap)
{
	Ps4KernelSystemCallHookArgument *uap2;

	//FIXME: should be lock and copyable
	//if(!uap->acquire(&(uap->mutex)->mtx_lock, MTX_UNOWNED, curthread))
	//	uap->lock(uap->mutex, curthread, 0, LOCK_FILE, LOCK_LINE);
	//mtx_lock(uap->mutex);

	// FIXME: Sleeping long should be avoided
 	uap2 = uap->allocate(sizeof(Ps4KernelSystemCallHookArgument), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadSystemCallHookMemoryCopy(uap, uap2, sizeof(Ps4KernelSystemCallHookArgument));

	uap2->hook = uap->allocate(uap->hookSize * sizeof(sy_call_t *), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadSystemCallHookMemoryCopy(uap->hook, uap2->hook, uap->hookSize * sizeof(sy_call_t *));

	uap2->hookType = uap->allocate(uap->hookSize * sizeof(int64_t), uap->mt, M_ZERO | M_WAITOK);
	ps4PayloadSystemCallHookMemoryCopy(uap->hookType, uap2->hookType, uap->hookSize * sizeof(int64_t));

	uap2->thread = td;
	uap2->uap = sysuap;

	//++uap->callCount; // FIXME: Make atomic

	//if(!uap->release(&(uap->mutex)->mtx_lock, curthread, MTX_UNOWNED))
	//	uap->unlock(uap->mutex, curthread, 0, LOCK_FILE, LOCK_LINE);
	//mtx_unlock(uap->mutex);

	return uap2;
}

int ps4PayloadSystemCallHookHandler(struct thread *td, Ps4KernelSystemCallHookArgument *uap)
{
	size_t i;
	int r;

	r = PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_CONTINUE;
	uap->sysret = 0;
	uap->returns[0] = 0;
	uap->returns[1] = 0;
	ps4PayloadSystemCallHookThreadSetReturn1(td, (register_t)uap);

	for(i = 0; i < uap->hookCount; ++i)
	{
		if(uap->hookType[i] == PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_PRE)
		{
			uap->hookTypeCurrent = PS4_KERNEL_FUNCTION_HOOK_TYPE_PRE;
			r = uap->hook[i](td, uap->uap);
		}
		else if(uap->hookType[i] == PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_PRE || uap->hookType[i] == PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_BOTH)
		{
			uap->hookTypeCurrent = PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_PRE;
			r = uap->hook[i](td, uap);
		}

		if(r != PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_CONTINUE)
			break;
	}

	if(r != PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_RETURN)
	{
		uap->sysret = uap->originalCall(td, uap->uap);
		uap->returns[0] = ps4PayloadSystemCallHookThreadGetReturn0(td);
		uap->returns[1] = ps4PayloadSystemCallHookThreadGetReturn1(td);

		uap->hookTypeCurrent = PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_POST;
		for(i = 0; i < uap->hookCount; ++i)
		{
			if(uap->hookType[i] == PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_POST || uap->hookType[i] == PS4_KERNEL_SYSTEM_CALL_HOOK_TYPE_GENERIC_BOTH)
				r = uap->hook[i](td, uap);

			if(r != PS4_KERNEL_SYSTEM_CALL_HOOK_CONTROL_CONTINUE)
				break;
		}

	}

	ps4PayloadSystemCallHookThreadSetReturn0(td, uap->returns[0]);
	ps4PayloadSystemCallHookThreadSetReturn1(td, uap->returns[1]);

	uap->free(uap->hookType, uap->mt);
	uap->free(uap->hook, uap->mt);
	uap->free(uap, uap->mt);

	return uap->sysret;
}
