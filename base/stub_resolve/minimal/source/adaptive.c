#include <stdint.h>
#include <ps4/stub_resolve.h>

static inline int ps4AdaptiveStubResolveIsInKernel()
{
	uintptr_t sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return (sp >> 48) > 0;
}

int ps4AdaptiveStubResolve(void *stub, char *moduleName, char *functionName, int *moduleHandle, void **userAddress, void **kernelAddress)
{
	if(ps4AdaptiveStubResolveIsInKernel())
		return ps4KernelStubResolve(stub, functionName, kernelAddress);
	return ps4StubResolve(stub, moduleName, functionName, moduleHandle, userAddress);
}
