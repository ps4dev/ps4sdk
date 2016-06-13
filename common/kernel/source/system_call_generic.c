#include <ps4/error.h>
#include <ps4/system_call.h>

// Self-contained -> copy-able
static inline register_t ps4SystemCallGenericThreadGetReturn0(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x380]));
}

static inline register_t ps4SystemCallGenericThreadGetReturn1(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x388]));
}

static inline void ps4SystemCallGenericThreadSetReturn0(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x380])) = ret;
}

/*
static inline void ps4SystemCallGenericThreadSetReturn1(struct thread *td, register_t ret)
{
	*((register_t *)&(((int8_t *)td)[0x388])) = ret;
}
*/

int ps4SystemCallGenericExecute(struct thread *td, Ps4SystemCallGenericExecuteArgument *uap)
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
		if(uap->returns[0] != NULL)
			*uap->returns[0] = ps4SystemCallGenericThreadGetReturn0(td);
		if(uap->returns[1] != NULL)
			*uap->returns[1] = ps4SystemCallGenericThreadGetReturn1(td);
	}

	return r;
}

int ps4SystemCallGenericCall(struct thread *td, Ps4SystemCallGenericCallArgument *uap)
{
	if(uap == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	if(uap->function == NULL) // Let's Ok this one too ... for symmetry
		return PS4_OK;

	uap->returns = ((register_t (*)())uap->function)(uap->rdi, uap->rsi, uap->rdx, uap->rcx, uap->r8, uap->r9);
	ps4SystemCallGenericThreadSetReturn0(td, uap->returns);
	//ps4SystemCallGenericThreadSetReturn1(td, 0);

	return PS4_OK;
}
