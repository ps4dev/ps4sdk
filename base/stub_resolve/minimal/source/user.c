#include <stddef.h>
#include <stdint.h>

#include <ps4/stub_resolve.h>

__asm__("\
	.pushsection .text \n \
	.global ps4StubResolveSystemCall \n \
	.type ps4StubResolveSystemCall, @function \n \
 	ps4StubResolveSystemCall:\n \
		movq $0, %rax \n \
		jmp ps4AssemblerSystemCall \n \
	.size ps4StubResolveSystemCall, .-ps4StubResolveSystemCall \n \
	.popsection \n \
");

int ps4StubResolveSystemCall();

int ps4StubResolve(void *stub, char *moduleName, char *functionName, int *moduleHandle, void **userAddress)
{
	static int (*ps4StubResolveLoadStartModule)(const char *name, size_t argc, const void *argv, uint32_t flags, void *a, void *b) = NULL;

	if(stub == NULL || moduleName == NULL || functionName == NULL || moduleHandle == NULL || userAddress == NULL)
		return -1;

	if(*moduleHandle <= 0) // for 0 (elf) we would do more then needed, but its a static null initialized value
	{
		if(!ps4StubResolveLoadStartModule)
		{
			int k = 0;
			ps4StubResolveSystemCall(594, "libkernel.sprx", 0, &k, 0);
			if(k <= 0)
				return -1;
			if(ps4StubResolveSystemCall(591, k, "sceKernelLoadStartModule", (void **)&ps4StubResolveLoadStartModule) != 0)
				return -1;
		}
		*moduleHandle = ps4StubResolveLoadStartModule(moduleName, 0, NULL, 0, NULL, NULL);
		if(*moduleHandle <= 0)
			return -1;
	}

	if(ps4StubResolveSystemCall(591, *moduleHandle, functionName, userAddress) != 0)
		return -1;

	return 0;
}
