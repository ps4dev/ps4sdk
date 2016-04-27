#include <ps4/kernel.h>
#include <ps4/kern.h>
#include <ps4/syscall.h>

// Let's not use cool and useful extensions ... stupid standard
#define Ps4KernelGenerateFunctionBodyReturning(name, returnType, ...) \
	if(ps4KernelIsInKernel()) \
		return name(__VA_ARGS__); \
	return (returnType)ps4KernelCall(name, __VA_ARGS__);

#define Ps4KernelGenerateFunctionZeroArgumnetsBodyReturning(name, returnType) \
	if(ps4KernelIsInKernel()) \
		return name(); \
	return (returnType)ps4KernelCall(name);

#define Ps4KernelGenerateFunctionBodyNotReturning(name, ...) \
	if(ps4KernelIsInKernel()) \
		name(__VA_ARGS__); \
	else \
		ps4KernelCall(name, __VA_ARGS__);

#define Ps4KernelGenerateFunctionZeroArgumnetsBodyNotReturning(name) \
	if(ps4KernelIsInKernel()) \
		name(); \
	else \
		ps4KernelCall(name);

void ps4KernelRegisterCr0Set(uint64_t cr0)
{
	ps4KernRegisterCr0Set(cr0);
}

uint64_t ps4KernelRegisterCr0Get(void)
{
	return ps4KernRegisterCr0Get();
}

int ps4KernelIsKernelAddress(void *address)
{
	return ps4KernIsKernelAddress(address);
}

int ps4KernelIsInKernel()
{
	return ps4KernIsInKernel();
}

int64_t ps4KernelCall_(void *call, int64_t rdi, int64_t rsi, int64_t rdx, int64_t rcx, int64_t r8, int64_t r9)
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

void *ps4KernelDlSym(char *symbol)
{
	Ps4KernelGenerateFunctionBodyReturning(ps4KernDlSym, void *, symbol)
}

void ps4KernelUartEnable()
{
	Ps4KernelGenerateFunctionZeroArgumnetsBodyNotReturning(ps4KernUartEnable)
}

void ps4KernelUartDisable()
{
	Ps4KernelGenerateFunctionZeroArgumnetsBodyNotReturning(ps4KernUartDisable)
}

void ps4KernelPrivilegeRoot()
{
	Ps4KernelGenerateFunctionZeroArgumnetsBodyNotReturning(ps4KernPrivilegeRoot)
}

int ps4KernelPrivilegeUnjail()
{
	Ps4KernelGenerateFunctionZeroArgumnetsBodyReturning(ps4KernPrivilegeUnjail, int)
}

int ps4KernelPrivilegeEscalate()
{
	Ps4KernelGenerateFunctionZeroArgumnetsBodyReturning(ps4KernPrivilegeEscalate, int)
}

void ps4KernelFunctionPatchToJumpTo(void *function, void *to)
{
	Ps4KernelGenerateFunctionBodyNotReturning(ps4KernFunctionPatchToJumpTo, function, to)
}

void ps4KernelFunctionPatchToTruth(void *function)
{
	Ps4KernelGenerateFunctionBodyNotReturning(ps4KernFunctionPatchToTruth, function)
}

int ps4KernelMemoryAllocate(void **memory, size_t size)
{
	Ps4KernelGenerateFunctionBodyReturning(ps4KernMemoryAllocate, int, memory, size)
}

void *ps4KernelMemoryMalloc(size_t size)
{
	Ps4KernelGenerateFunctionBodyReturning(ps4KernMemoryMalloc, void *, size)
}

void ps4KernelMemoryFree(void *addr)
{
	Ps4KernelGenerateFunctionBodyNotReturning(ps4KernMemoryFree, addr)
}

void ps4KernelMemoryCopy(void *src, void *dest, size_t size)
{
	Ps4KernelGenerateFunctionBodyNotReturning(ps4KernMemoryCopy, src, dest, size)
}
