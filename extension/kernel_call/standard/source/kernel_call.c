#define _KERNEL

#include <ps4/kernel.h>
#include <ps4/system_call.h>

int64_t ps4KernelCallPrivate(void *call, int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9)
{
	Ps4SystemCallGenericCallArgument uap;
	int64_t ret;

	if(ps4KernelIsInKernel() == PS4_OK)
		return ((int64_t (*)())call)(rdi, rsi, rdx, rcx, r8, r9);

	uap.function = (void *)call;
	uap.rdi = (register_t)rdi;
	uap.rsi = (register_t)rsi;
	uap.rdx = (register_t)rdx;
	uap.rcx = (register_t)rcx;
	uap.r8 = (register_t)r8;
	uap.r9 = (register_t)r9;
	uap.returns = 0;
	ps4KernelExecute((void *)ps4SystemCallGenericCall, &uap, &ret, NULL);

	return ret;
}

__asm__("\
	.pushsection .text \n \
	.global ps4KernelCall \n \
	.type ps4KernelCall, @function \n \
 	ps4KernelCall:\n \
		jmp ps4KernelCallPrivate \n \
	.size ps4KernelCall, .-ps4KernelCall \n \
	.popsection \n \
");
