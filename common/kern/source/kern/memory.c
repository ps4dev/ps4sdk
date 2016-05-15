#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/malloc.h>

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <bsm/audit_kevents.h>

#include <elf.h>
#include <machine/specialreg.h>

#include <ps4/kern.h>

int ps4KernMemoryAllocate(void **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	if(size <= 0x4000) // pagesize
		size = 0x4001; // force new

	ps4KernProtectionExecuteDisable();
	*memory = malloc(size, ps4KernDlSym("M_TEMP"), M_ZERO | M_WAITOK);
	ps4KernProtectionExecuteEnable();

	return PS4_OK;
}

void *ps4KernMemoryMalloc(size_t size)
{
	void *m;

	if(ps4KernMemoryAllocate(&m, size) != PS4_OK)
		return NULL;

	return m;
}

int ps4KernMemoryFree(void *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	free(memory, ps4KernDlSym("M_TEMP"));

	return PS4_OK;
}

int ps4KernMemoryCopy(void *from, void *to, size_t size)
{
	if(from == NULL || to == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernProtectionWriteDisable();
	memcpy(to, from, size);
	ps4KernProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernMemorySwap(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernProtectionWriteDisable();
	for(size_t i = 0; i < size; ++i)
	{
		uint8_t t = at[i];
		at[i] = bt[i];
		bt[i] = t;
	}
	ps4KernProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernMemoryFill(void *memory, uint8_t byte, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernProtectionWriteDisable();
	memset(memory, (int)byte, size);
	ps4KernProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernMemoryCompareNonZeros(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	for(size_t i = 0; i < size; ++i)
		if(at[i] != 0 && at[i] != bt[i])
			return at[i] - bt[i];

	return PS4_OK;
}
