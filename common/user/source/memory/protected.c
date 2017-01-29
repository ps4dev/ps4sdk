#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>

#include <sce/kernel.h>

#include <ps4/memory/protected.h>
#include <ps4/error.h>

typedef struct Ps4MemoryProtected
{
	void *writable;
	void *executable;
	size_t size;
}
Ps4MemoryProtected;

int ps4MemoryProtectedCreate(Ps4MemoryProtected **memory, size_t size)
{
	int executableHandle, writableHandle;
	Ps4MemoryProtected *m;
	long pageSize = sysconf(_SC_PAGESIZE);

	if(memory == NULL)
 		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == 0)
		return PS4_ERROR_ARGUMENT_SIZE_NULL;

	m = (Ps4MemoryProtected *)malloc(sizeof(Ps4MemoryProtected));
	if(m == NULL)
		return PS4_ERROR_OUT_OF_MEMORY;

	m->size = (size / pageSize + 1) * pageSize; // align to pageSize

	sceKernelJitCreateSharedMemory(0, m->size, PROT_READ | PROT_WRITE | PROT_EXEC, &executableHandle);
	if(executableHandle == 0)
		goto e1;
	sceKernelJitCreateAliasOfSharedMemory(executableHandle, PROT_READ | PROT_WRITE, &writableHandle);
	if(writableHandle == 0)
		goto e2;
	//sceKernelJitMapSharedMemory(m->writableHandle, PROT_CPU_READ | PROT_CPU_WRITE, &writable);
	m->executable = mmap(NULL, m->size, PROT_READ | PROT_EXEC, MAP_SHARED, executableHandle, 0);
	if(m->executable == MAP_FAILED)
		goto e3;
	m->writable = mmap(NULL, m->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_TYPE, writableHandle, 0);
	if(m->writable == MAP_FAILED)
		goto e4;

	close(executableHandle);
	close(writableHandle);

	*memory = m;
	return PS4_OK;

	e4:
		munmap(m->executable, m->size);
	e3:
		close(writableHandle);
	e2:
		close(executableHandle);
	e1:
		free(m);

	return PS4_ERROR_OUT_OF_MEMORY; // make error codes proper errnos ... everywhere ... meh
}

int ps4MemoryProtectedDestroy(Ps4MemoryProtected *memory)
{
	int r = 0;
	if(memory == NULL)
		return -1;
	r |= munmap(memory->writable, memory->size);
	r |= munmap(memory->executable, memory->size);
	free(memory);
	return r;
}

int ps4MemoryProtectedGetWritableAddress(Ps4MemoryProtected *memory, void **address)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(address == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*address = memory->writable;
	return PS4_OK;
}

int ps4MemoryProtectedGetExecutableAddress(Ps4MemoryProtected *memory, void **address)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(address == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*address = memory->executable;
	return PS4_OK;
}

int ps4MemoryProtectedGetSize(Ps4MemoryProtected *memory, size_t *size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = memory->size;
	return PS4_OK;
}
