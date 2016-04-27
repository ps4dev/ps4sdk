#pragma once

#include <stddef.h>

#include <ps4/memory.h>

typedef struct Ps4MemoryProtected Ps4MemoryProtected;

int ps4MemoryProtectedCreate(Ps4MemoryProtected **memory, size_t size);
int ps4MemoryProtectedDestroy(Ps4MemoryProtected *memory);
void *ps4MemoryProtectedGetWritableAddress(Ps4MemoryProtected *memory);
void *ps4MemoryProtectedGetExecutableAddress(Ps4MemoryProtected *memory);
size_t ps4MemoryProtectedGetSize(Ps4MemoryProtected *memory);
