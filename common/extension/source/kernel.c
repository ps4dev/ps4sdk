#include <stdlib.h>
//#include <stdio.h>
#include <stdint.h>

#include <ps4/kernel.h>
#include <ps4/kern.h>
#include <ps4/syscall.h>
#include <ps4/machine.h>

#include <machine/specialreg.h>

int ps4KernelIsKernelAddress(void *address)
{
	return ps4KernIsKernelAddress(address);
}

int ps4KernelIsInKernel()
{
	return ps4KernIsInKernel();
}

int ps4KernelBrewCoffee()
{
	return PS4_ERROR_I_AM_A_TEAPOT;
}

int64_t ps4KernelCallBase(void *call, int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9)
{
	Ps4SyscallGenericCallArgument uap = {0};
	int64_t ret;

	if(ps4KernelIsInKernel())
		return ((int64_t (*)())call)(rdi, rsi, rdx, rcx, r8, r9);

	uap.function = (void *)call;
	uap.rdi = (register_t)rdi;
	uap.rsi = (register_t)rsi;
	uap.rdx = (register_t)rdx;
	uap.rcx = (register_t)rcx;
	uap.r8 = (register_t)r8;
	uap.r9 = (register_t)r9;
	ps4KernelExecute((void *)ps4SyscallGenericCall, &uap, &ret, NULL);

	return ret;
}

/* Ah systemv + asm ... <3 */
__asm__("\
	.pushsection .text \n \
	.global ps4KernelCall \n \
	.type ps4KernelCall, @function \n \
 	ps4KernelCall:\n \
		jmp ps4KernelCallBase \n \
	.size ps4KernelCall, .-ps4KernelCall \n \
	.popsection \n \
");

void ps4KernelRegisterCr0Set(uint64_t cr0)
{
	ps4KernelCall((void *)ps4KernRegisterCr0Set, cr0);
}

uint64_t ps4KernelRegisterCr0Get(void)
{
	return (uint64_t)ps4KernelCall((void *)ps4KernRegisterCr0Get);
}

int ps4KernelMachineInstructionNext(void *base, size_t *next)
{
	// currently this puppy does not run in kernel :(
	//return (int)ps4KernelCall(ps4MachineInstructionNext, base, next);
 	if(ps4KernelIsInKernel())
		return PS4_ERROR_NOT_IMPLEMENTED;

	void *m = malloc(128);
	ps4KernelMemoryCopy(base, m, 128);
	int r = ps4MachineInstructionSeek(m, next);
	free(m);
	return r;
	//return ps4MachineInstructionSeek(base, next);
}

int ps4KernelMachineInstructionSeek(void *base, size_t *offset)
{
	// currently this puppy does not run in kernel :(
	//return (int)ps4KernelCall(ps4MachineInstructionSeek, base, offset);
 	if(ps4KernelIsInKernel())
		return PS4_ERROR_NOT_IMPLEMENTED;

	void *m = malloc(128);
	ps4KernelMemoryCopy(base, m, 128);
	int r = ps4MachineInstructionSeek(m, offset);
	free(m);
	return r;
	//return ps4MachineInstructionSeek(base, offset);
}

void *ps4KernelDlSym(char *symbol)
{
	return (void *)ps4KernelCall((void *)ps4KernDlSym, symbol);
}

int ps4KernelUartEnable()
{
	return (int)ps4KernelCall((void *)ps4KernUartEnable);
}

int ps4KernelUartDisable()
{
	return (int)ps4KernelCall((void *)ps4KernUartDisable);
}

int ps4KernelPrivilegeUnjail()
{
	return (int)ps4KernelCall((void *)ps4KernPrivilegeUnjail);
}

int ps4KernelPrivilegeEscalate()
{
	return (int)ps4KernelCall((void *)ps4KernPrivilegeEscalate);
}

void ps4KernelFunctionPatchToJump(void *function, void *to)
{
	ps4KernelCall((void *)ps4KernFunctionPatchToJump, function, to);
}

void ps4KernelFunctionPatchToReturn(void *function, int64_t value)
{
	ps4KernelCall((void *)ps4KernFunctionPatchToReturn, function, value);
}

int ps4KernelMemoryAllocate(void **memory, size_t size)
{
	return (int)ps4KernelCall((void *)ps4KernMemoryAllocate, memory, size);
}

void *ps4KernelMemoryMalloc(size_t size)
{
	return (void *)ps4KernelCall((void *)ps4KernMemoryMalloc, size);
}

void ps4KernelMemoryFree(void *addr)
{
	ps4KernelCall((void *)ps4KernMemoryFree, addr);
}

void ps4KernelMemoryCopy(void *src, void *dest, size_t size)
{
	ps4KernelCall((void *)ps4KernMemoryCopy, src, dest, size);
}

//int ps4KernelFunctionHookCreate(Ps4KernelFunctionHook **hookh, void *function, void *hook);

int ps4KernelFunctionHookCreateSized(Ps4KernelFunctionHook **hookh, void *function, void *hook, size_t size)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionHookCreateSized, hookh, function, hook, size);
}
int ps4KernelFunctionHookDestroy(Ps4KernelFunctionHook *hookh)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionHookDestroy, hookh);
}
int ps4KernelFunctionHookAdd(Ps4KernelFunctionHook *hookh, void *hook)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionHookAdd, hookh, hook);
}
int ps4KernelFunctionHookRemove(Ps4KernelFunctionHook *hookh, void *hook)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionHookRemove, hookh, hook);
}

//int ps4KernelFunctionHook(void *function, void *hook, Ps4KernelFunctionHook **hookh);
int ps4KernelFunctionHookSized(void *function, void *hook, size_t size)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionHookSized, function, hook, size);
}
int ps4KernelFunctionIsHooked(void *function)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionIsHooked, function);
}
int ps4KernelFunctionGetHook(void *function, Ps4KernelFunctionHook **hook)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionGetHook, function, hook);
}
int ps4KernelFunctionUnhook(void *function)
{
	return (int)ps4KernelCall((void *)ps4KernFunctionUnhook, function);
}
