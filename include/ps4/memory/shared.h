#pragma once

#include <stddef.h>

#include <ps4/memory.h>

typedef struct Ps4MemoryShared Ps4MemoryShared;

int ps4MemorySharedOpen(Ps4MemoryShared **memory, size_t size, const char *path);
int ps4MemorySharedClose(Ps4MemoryShared *memory);
int ps4MemorySharedUnlink(Ps4MemoryShared *memory);
int ps4MemorySharedGetAddress(Ps4MemoryShared *memory, void **address);
int ps4MemorySharedGetSize(Ps4MemoryShared *memory, size_t *size);
