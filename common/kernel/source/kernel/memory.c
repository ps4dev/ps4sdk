#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <sys/malloc.h>

#include <ps4/kernel.h>

static struct malloc_type *ps4KernelMemoryType;

int ps4KernelMemoryInitialize()
{
	ps4KernelMemoryType = ps4KernelDlSym("M_TEMP");
	return PS4_OK;
}

int ps4KernelMemoryAllocate(void **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	if(size <= 0x4000) // pagesize
		size = 0x4001; // force new

	if(ps4KernelMemoryType == NULL)
		ps4KernelMemoryInitialize();

	ps4KernelProtectionExecuteDisable();
	*memory = malloc(size, ps4KernelMemoryType, M_ZERO | M_WAITOK);
	ps4KernelProtectionExecuteEnable();

	return PS4_OK;
}

int ps4KernelMemoryAllocateData(void **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	if(ps4KernelMemoryType == NULL)
		ps4KernelMemoryInitialize();

	*memory = malloc(size, ps4KernelMemoryType, M_ZERO | M_WAITOK);

	return PS4_OK;
}

int ps4KernelMemoryReallocateData(void **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	if(ps4KernelMemoryType == NULL)
		ps4KernelMemoryInitialize();

	*memory = realloc(*memory, size, ps4KernelMemoryType, M_ZERO | M_WAITOK);

	return PS4_OK;
}

int ps4KernelMemoryAllocateString(char **string, size_t *size, size_t sizeMax, const char *format, ...)
{
	va_list args;
	int r;

	if(string == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	va_start(args, format);
	r = ps4KernelMemoryAllocateStringWithArgumentList(string, size, sizeMax, format, args);
	va_end(args);

	return r;
}

int ps4KernelMemoryAllocateStringWithArgumentList_(char **string, size_t *size, size_t sizeMax, int partial, const char *format, va_list args)
{
	va_list vargs;
	size_t sz;
	char *s;

	if(string == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL || format == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;
	if(sizeMax + 1 < sizeMax)
		return PS4_ERROR_ARGUMENT_SIZE_INVALID;
	if(sizeMax != 0)
		++sizeMax;

	va_copy(vargs, args);
	sz = vsnprintf(NULL, sizeMax, format, vargs);
	va_end(vargs);

	// do not alloc for "\0" or larger then sizeMax (half-allocs)
	if(sz == 0)
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
 	if(partial == 0 && sizeMax > 0 && sz == sizeMax)
		return PS4_ERROR_ARGUMENT_SIZE_TOO_SMALL;

	//s = ps4KernelMemoryMalloc(sz + 1);
	ps4KernelMemoryAllocateData((void **)&s, sz + 1);
	if(s == NULL)
		return PS4_ERROR_KERNEL_OUT_OF_MEMORY;

	vsnprintf(s, sz + 1, format, args);
	s[sz] = '\0';

	if(size != NULL)
		*size = sz;
	*string = s;

	return PS4_OK;
}

int ps4KernelMemoryAllocateStringWithArgumentList(char **string, size_t *size, size_t sizeMax, const char *format, va_list args)
{
	return ps4KernelMemoryAllocateStringWithArgumentList_(string, size, sizeMax, 0, format, args);
}

int ps4KernelMemoryAllocateStringPartialWithArgumentList(char **string, size_t *size, size_t sizeMax, const char *format, va_list args)
{
	return ps4KernelMemoryAllocateStringWithArgumentList_(string, size, sizeMax, 1, format, args);
}

void *ps4KernelMemoryMalloc(size_t size)
{
	void *m;

	if(ps4KernelMemoryAllocate(&m, size) != PS4_OK)
		return NULL;

	return m;
}

void *ps4KernelMemoryMallocData(size_t size)
{
	void *m;

	if(ps4KernelMemoryAllocateData(&m, size) != PS4_OK)
		return NULL;

	return m;
}

int ps4KernelMemoryFree(void *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernelMemoryType == NULL)
		ps4KernelMemoryInitialize();

	free(memory, ps4KernelMemoryType);

	return PS4_OK;
}

int ps4KernelMemoryCopy(void *from, void *to, size_t size)
{
	if(from == NULL || to == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernelProtectionWriteDisable();
	memcpy(to, from, size);
	ps4KernelProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernelMemorySwap(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernelProtectionWriteDisable();
	for(size_t i = 0; i < size; ++i)
	{
		uint8_t t = at[i];
		at[i] = bt[i];
		bt[i] = t;
	}
	ps4KernelProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernelMemoryFill(void *memory, uint8_t byte, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernelProtectionWriteDisable();
	memset(memory, (int)byte, size);
	ps4KernelProtectionWriteEnable();

	return PS4_OK;
}

int ps4KernelMemoryCompareNonZeros(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	for(size_t i = 0; i < size; ++i)
		if(at[i] != 0 && at[i] != bt[i])
			return at[i] - bt[i];

	return PS4_OK;
}
