#include <stddef.h>

#include <ps4/kernel.h>
#include <ps4/stub_resolve.h>

int ps4KernelStubResolve(void *stub, char *functionName, void **kernelAddress)
{
	if(stub == NULL || functionName == NULL || kernelAddress == NULL)
		return -1;

	*kernelAddress = ps4KernelDlSym(functionName);
	if(*kernelAddress == NULL)
		return -1;

	return 0;
}
