#pragma once

int ps4KernMemoryAllocate(void **memory, size_t size);
void *ps4KernMemoryMalloc(size_t size);
void ps4KernMemoryFree(void *memory);
void ps4KernMemoryCopy(void *from, void *to, size_t size);
