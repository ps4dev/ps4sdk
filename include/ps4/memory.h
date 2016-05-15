#pragma once

#include <ps4/memory/protected.h>
#include <ps4/memory/shared.h>

typedef struct Ps4Memory Ps4Memory;

int ps4MemoryAllocate(void **memory, size_t size);
void *ps4MemoryMalloc(size_t size);
int ps4MemoryFree(void *memory);
int ps4MemoryCreate(Ps4Memory **memory, size_t size);
int ps4MemoryDestroy(Ps4Memory *memory);
void *ps4MemoryGetAddress(Ps4Memory *memory);
size_t ps4MemoryGetSize(Ps4Memory *memory);
