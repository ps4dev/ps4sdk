#include <sys/cdefs.h>
#include <sys/types.h>

#include <machine/cpufunc.h>

#include <ps4/kernel.h>

int ps4KernelThreadGetCurrent(Ps4KernelThread **td)
{
	Ps4KernelThread *t;
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	__asm__ volatile("movq %%gs:0, %0;" : "=r"(t));
	*td = t;
	return PS4_OK;
}

// FIXME: To C once td tested / fixed
int ps4KernelThreadGetReturn(Ps4KernelThread *td, register_t *ret)
{
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(ret == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	//return td->retval[0];
	*ret = *((register_t *)&(((int8_t *)td)[0x380]));
	return PS4_OK;
}

int ps4KernelThreadGetPrimaryReturn(Ps4KernelThread *td, register_t *ret)
{
	return ps4KernelThreadGetReturn(td, ret);
}

int ps4KernelThreadGetSecondaryReturn(Ps4KernelThread *td, register_t *ret)
{
	//return td->retval[1];
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(ret == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*ret = *((register_t *)&(((int8_t *)td)[0x388]));
	return PS4_OK;
}

int ps4KernelThreadSetReturn(Ps4KernelThread *td, register_t ret)
{
	//return td->retval[0];
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	*((register_t *)&(((int8_t *)td)[0x380])) = ret;
	return PS4_OK;
}

int ps4KernelThreadSetPrimaryReturn(Ps4KernelThread *td, register_t ret)
{
	return ps4KernelThreadSetReturn(td, ret);
}

int ps4KernelThreadSetSecondaryReturn(Ps4KernelThread *td, register_t ret)
{
	//return td->retval[1];
	if(td == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	*((register_t *)&(((int8_t *)td)[0x388])) = ret;
	return PS4_OK;
}
