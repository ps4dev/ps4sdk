#pragma once

#include <stddef.h>

#include <ps4/memory.h>

typedef struct Ps4MemoryShared Ps4MemoryShared;

int ps4MemorySharedOpen(Ps4MemoryShared **memory, const char *path, size_t size);
int ps4MemorySharedClose(Ps4MemoryShared *memory);
int ps4MemorySharedUnlink(Ps4MemoryShared *memory);
void *ps4MemorySharedGetAddress(Ps4MemoryShared *memory);
size_t ps4MemorySharedGetSize(Ps4MemoryShared *memory);
