#define _KERNEL

#include <ps4/kern.h>
#include <machine/cpufunc.h>

void ps4KernRegisterCr0Set(register_t cr0)
{
	load_cr0(cr0);
}

register_t ps4KernRegisterCr0Get(void)
{
	return rcr0();
}

void ps4KernRegisterMsrSet(register_t msr, register_t value)
{
	wrmsr(msr, value);
}

register_t ps4KernRegisterMsrGet(register_t msr)
{
	return rdmsr(msr);
}

int ps4KernBrewCoffee()
{
	return PS4_ERROR_I_AM_A_TEAPOT;
}

struct thread *ps4KernThreadCurrent(void)
{
	struct thread *td;
	__asm__ volatile("movq %%gs:0, %0;" : "=r"(td));
	return td;
}

// FIXME: To C once td tested / safe
register_t ps4KernThreadGetReturn0(struct thread *td)
{
	//struct thread *td = ps4KernThreadCurrent();
	//return td->retval[0];
	return *((register_t *)&(((int8_t *)td)[0x380]));
}

register_t ps4KernThreadGetReturn1(struct thread *td)
{
	//struct thread *td = ps4KernThreadCurrent();
	//return td->retval[1];
	return *((register_t *)&(((int8_t *)td)[0x388]));
}

void ps4KernThreadSetReturn0(struct thread *td, register_t ret)
{
	//struct thread *td = ps4KernThreadCurrent();
	//return td->retval[0];
	*((register_t *)&(((int8_t *)td)[0x380])) = ret;
}

void ps4KernThreadSetReturn1(struct thread *td, register_t ret)
{
	//struct thread *td = ps4KernThreadCurrent();
	//return td->retval[1];
	*((register_t *)&(((int8_t *)td)[0x388])) = ret;
}

int ps4KernIsKernelAddress(void *address)
{
	return ((uintptr_t)address >> 48) > 0;
}

int ps4KernIsInKernel()
{
	void *sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return ps4KernIsKernelAddress(sp);
}
