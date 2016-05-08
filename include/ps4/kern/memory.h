#pragma once

uint8_t *ps4HackKernMemoryGetBaseAddress();

int ps4KernMemoryAllocate(void **memory, size_t size);
void *ps4KernMemoryMalloc(size_t size);
void ps4KernMemoryFree(void *memory);
int ps4KernMemoryCopy(void *from, void *to, size_t size);
int ps4KernMemorySwap(void *a, void *b, size_t size);
int ps4KernMemoryCompareNonZeros(void *a, void *b, size_t size);
