#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>

#include <kernel.h>

#include <ps4/memory.h>
#include <ps4/kernel.h>
#include <ps4/error.h>

typedef struct Ps4Memory
{
	size_t size;
	void *address;
}
Ps4Memory;

/*
#ifndef MAP_TYPE
	#define MAP_TYPE 0x0f
#endif
*/

int ps4MemoryAllocate(void **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

/*
	if(size <= 0x4000) // page size
		size = 0x4001; // force new

	if(size % 0x4000 != 0)
		size = 0x4000 * ((size / 0x4000) + 1);

	ps4KernelProtectionExecuteDisable();
	// *memory = malloc(size);
	*memory = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_TYPE, 0, 0);
	mprotect(*memory, size, PROT_READ | PROT_WRITE | PROT_EXEC);
	ps4KernelProtectionExecuteEnable();
*/
	*memory = malloc(size);

	return PS4_OK;
}

void *ps4MemoryMalloc(size_t size)
{
	void *m;
	if(ps4MemoryAllocate(&m, size) != PS4_OK)
		return NULL;
	return m;
}

int ps4MemoryFree(void *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	free(memory);

	return PS4_OK;
}

int ps4MemoryCreate(Ps4Memory **memory, size_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	*memory = malloc(sizeof(Ps4Memory));
	(*memory)->size = size;
	(*memory)->address = ps4MemoryMalloc(size);

	return PS4_OK;
}

int ps4MemoryDestroy(Ps4Memory *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(memory->address != NULL)
		ps4MemoryFree(memory->address);
	free(memory);

	return PS4_OK;
}

void *ps4MemoryGetAddress(Ps4Memory *memory)
{
	if(memory == NULL)
		return NULL;
	return memory->address;
}

size_t ps4MemoryGetSize(Ps4Memory *memory)
{
	if(memory == NULL)
		return 0;
	return memory->size;
}
