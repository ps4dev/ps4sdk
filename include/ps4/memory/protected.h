#pragma once

#include <stddef.h>

#include <ps4/memory.h>

typedef struct Ps4MemoryProtected Ps4MemoryProtected;

int ps4MemoryProtectedCreate(Ps4MemoryProtected **memory, size_t size);
int ps4MemoryProtectedDestroy(Ps4MemoryProtected *memory);
int ps4MemoryProtectedGetWritableAddress(Ps4MemoryProtected *memory, void **address);
int ps4MemoryProtectedGetExecutableAddress(Ps4MemoryProtected *memory, void **address);
int ps4MemoryProtectedGetSize(Ps4MemoryProtected *memory, size_t *size);
