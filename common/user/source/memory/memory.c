#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/stat.h>

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

	ps4KernelelProtectionExecuteDisable();
	// *memory = malloc(size);
	*memory = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_TYPE, 0, 0);
	mprotect(*memory, size, PROT_READ | PROT_WRITE | PROT_EXEC);
	ps4KernelelProtectionExecuteEnable();
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

int ps4MemoryGetAddress(Ps4Memory *memory, void **address)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(address == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*address = memory->address;
	return PS4_OK;
}

int ps4MemoryGetSize(Ps4Memory *memory, size_t *size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = memory->size;
	return PS4_OK;
}


int ps4MemoryAllocateFromFileWithoutSize(void **memory, size_t *size, int fd)
{
	int length = 0;
	int full = 4096;
	uint8_t *data = (void *)malloc(full);
	size_t s = 0;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	while((length = read(fd, data + s, full - s)) > 0)
	{
		s += length;
		if(s == full)
		{
			void *t;
			full *= 2;
			t = realloc(data, full);
			if(t == NULL)
			{
				free(data);
				return -1;
			}
			data = t;
		}
	}

	if(size != NULL)
		*size = s;

	*memory = data;

	return PS4_OK;
}

int ps4MemoryAllocateFileFromPathAligned(void **memory, size_t *size, char *file, size_t alignment)
{
	struct stat s;
	FILE *f;
	uint32_t *b;
	size_t sz;
	size_t i;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(stat(file, &s) < 0)
		return PS4_ERROR_NOT_FOUND;

	if(alignment == 0)
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

 	sz = ((size_t)s.st_size * alignment) / alignment;
	b = (uint32_t *)malloc(sz * sizeof(uint8_t));

	if(b == NULL)
		return PS4_ERROR_OUT_OF_MEMORY;

	f = fopen(file, "rb");
	if(f == NULL)
	{
		free(b);
		return PS4_ERROR_FORBIDDEN;
	}
	fread(b, s.st_size, 1, f);
	fclose(f);

	if(size != NULL)
		*size = sz;

	for(i = s.st_size; i < sz; ++i)
		((uint32_t *)b)[i] = 0;

	*memory = b;

	return PS4_OK;
}
