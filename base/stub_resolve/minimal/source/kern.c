#include <stddef.h>

#include <ps4/kern.h>
#include <ps4/stub_resolve.h>

int ps4KernStubResolve(void *stubAddress, char *functionName, void **kernelAddress)
{
	if(stubAddress == NULL || functionName == NULL || kernelAddress == NULL)
		return -1;

	*kernelAddress = ps4KernDlSym(functionName);
	if(*kernelAddress == NULL)
		return -1;

	return 0;
}
