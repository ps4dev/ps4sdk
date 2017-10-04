#pragma once

#include <stdarg.h>

int ps4KernelMemoryAllocate(void **memory, size_t size);
int ps4KernelMemoryAllocateData(void **memory, size_t size);
int ps4KernelMemoryReallocateData(void **memory, size_t size);
void *ps4KernelMemoryMalloc(size_t size);
void *ps4KernelMemoryMallocData(size_t size);
int ps4KernelMemoryAllocateString(char **string, size_t *size, size_t sizeMax, const char *format, ...);
int ps4KernelMemoryAllocateStringWithArgumentList(char **string, size_t *size, size_t sizeMax, const char *format, va_list args);
int ps4KernelMemoryFree(void *memory);
int ps4KernelMemoryCopy(void *from, void *to, size_t size);
int ps4KernelMemorySwap(void *a, void *b, size_t size);
int ps4KernelMemoryFill(void *memory, uint8_t byte, size_t size);
int ps4KernelMemoryCompareNonZeros(void *a, void *b, size_t size);
