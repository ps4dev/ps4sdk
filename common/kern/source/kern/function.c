#define _KERNEL

#include <ps4/kern.h>
#include <ps4/payload.h>

void ps4KernFunctionPatchToTruth(void *function)
{
	ps4KernMemoryCopy((void *)ps4PayloadTruth, function, ps4PayloadTruthSize);
}

int ps4KernFunctionPatchToJumpTo(void *function, void *to)
{
	if(ps4KernIsKernelAddress(function) != ps4KernIsKernelAddress(to))
		return -1; // no space shifting, el psy kongroo
	ps4KernFunctionPatchToJumpToUnsafe(function, to);
	return 0;
}

void ps4KernFunctionPatchToJumpToUnsafe(void *function, void *to)
{
	ps4KernMemoryCopy((void *)ps4PayloadStubJump, function, ps4PayloadStubJumpSize);
	ps4KernMemoryCopy(&to, (int8_t *)function + 2, sizeof(void *));
}

int ps4KernFunctionHook(void *function, void *hook)
{
	return -1;
}

int ps4KernFunctionUnhook(void *function, void *hook)
{
	return -1;
}
