#include <stdint.h>
#include <ps4/stub_resolve.h>

static int ps4MixedStubResolveEarlyIsInKernel()
{
	uintptr_t sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return (sp >> 48) > 0;
}

int ps4MixedStubResolve(void *stubAddress, char *moduleName, char *functionName, int *moduleHandle, void **userAddress, void **kernelAddress)
{
	if(ps4MixedStubResolveEarlyIsInKernel())
		return ps4KernStubResolve(stubAddress, functionName, kernelAddress);
	return ps4StubResolve(stubAddress, moduleName, functionName, moduleHandle, userAddress);
}
