#pragma once

#include <stddef.h>

#include <ps4/memory.h>

typedef struct Ps4Memory Ps4Memory;

int ps4UserMemoryAllocate(void **memory, size_t size);
void *ps4UserMemoryMalloc(size_t size);
int ps4UserMemoryFree(void *memory);

int ps4MemoryAllocate(void **memory, size_t size);
int ps4MemoryAllocateData(void **memory, size_t size);
void *ps4MemoryMalloc(size_t size);
int ps4MemoryFree(void *memory);

/*
int ps4MemoryCreate(Ps4Memory **memory, size_t size);
int ps4MemoryDestroy(Ps4Memory *memory);
int ps4MemoryGetAddress(Ps4Memory *memory, void **address);
int ps4MemoryGetSize(Ps4Memory *memory, size_t *size);
*/

int ps4MemoryAllocateFromFileWithoutSize(void **memory, size_t *size, int fd);
int ps4MemoryAllocateFileFromPathAligned(void **memory, size_t *size, char *file, size_t alignment);
