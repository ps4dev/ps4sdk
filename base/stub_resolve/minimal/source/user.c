#include <stddef.h>
#include <stdint.h>

#include <ps4/stub_resolve.h>

__asm__("\
	.pushsection .text \n \
	.global ps4StubResolveEarlySyscall \n \
	.type ps4StubResolveEarlySyscall, @function \n \
 	ps4StubResolveEarlySyscall:\n \
		movq $0, %rax \n \
		jmp ps4AssemblerSyscall \n \
	.size ps4StubResolveEarlySyscall, .-ps4StubResolveEarlySyscall \n \
	.popsection \n \
");

int ps4StubResolveEarlySyscall();

int ps4StubResolve(void *stubAddress, char *moduleName, char *functionName, int *moduleHandle, void **userAddress)
{
	static int (*ps4StubResolveEarlyLoadStartModule)(const char *name, size_t argc, const void *argv, uint32_t flags, void *a, void *b) = NULL;

	if(stubAddress == NULL || moduleName == NULL || functionName == NULL || moduleHandle == NULL || userAddress == NULL)
		return -1;

	if(*moduleHandle <= 0)
	{
		if(!ps4StubResolveEarlyLoadStartModule)
		{
			int k = 0;
			ps4StubResolveEarlySyscall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				return -1;
			if(ps4StubResolveEarlySyscall(591, k, "sceKernelLoadStartModule", (void **)&ps4StubResolveEarlyLoadStartModule) != 0)
				return -1;
		}
		*moduleHandle = ps4StubResolveEarlyLoadStartModule(moduleName, 0, NULL, 0, NULL, NULL);
		if(*moduleHandle <= 0)
			return -1;
	}

	if(ps4StubResolveEarlySyscall(591, *moduleHandle, functionName, userAddress) != 0)
		return -1;

	return 0;
}
