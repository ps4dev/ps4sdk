#include <ps4/kernel.h>

int ps4KernelBrewCoffee()
{
	return PS4_ERROR_I_AM_A_TEAPOT;
}

int ps4KernelIsKernelAddress(void *address)
{
	if(((uintptr_t)address >> 48) > 0)
		return PS4_OK;
	return PS4_NOT_OK;
}

int ps4KernelIsInKernel()
{
	void *sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return ps4KernelIsKernelAddress(sp);
}
