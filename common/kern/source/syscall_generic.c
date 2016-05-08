#include <ps4/error.h>
#include <ps4/syscall.h>

// Self-contained -> copy-able
static inline register_t ps4SyscallGenericThreadGetReturn0(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x380]));
}

static inline register_t ps4SyscallGenericThreadGetReturn1(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x388]));
}

static inline void ps4SyscallGenericThreadSetReturn0(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x380])) = ret;
}

/*
static inline void ps4SyscallGenericThreadSetReturn1(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x388])) = ret;
}
*/

int ps4SyscallGenericExecute(struct thread *td, Ps4SyscallGenericExecuteArgument *uap)
{
	int r;

	// We hold these truths to be self-evident, but checking 'em is probably good too. O´´* <(^^<)
	if(uap == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if(uap->function == NULL) // installed syscall arg 1
		return PS4_OK; // This is an intended OK nop

	r = ((int (*)())uap->function)(td, uap->uap);
	if(r == PS4_OK)
	{
		if(uap->returned[0] != NULL)
			*uap->returned[0] = ps4SyscallGenericThreadGetReturn0(td);
		if(uap->returned[1] != NULL)
			*uap->returned[1] = ps4SyscallGenericThreadGetReturn1(td);
	}

	return r;
}

int ps4SyscallGenericCall(struct thread *td, Ps4SyscallGenericCallArgument *uap)
{
	if(uap == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if(uap->function == NULL) // Let's Ok this one too ... for symmetry
		return PS4_OK;

	uap->returned = ((register_t (*)())uap->function)(uap->rdi, uap->rsi, uap->rdx, uap->rcx, uap->r8, uap->r9);
	ps4SyscallGenericThreadSetReturn0(td, uap->returned);
	//ps4SyscallGenericThreadSetReturn1(td, 0);

	return PS4_OK;
}
