#pragma once

#include <stdlib.h>
#include <stdio.h>

int ps4MemoryAllocateFileWithoutSize(void **memory, size_t *size, int fd);
int ps4MemoryAllocateFileFromPathAligned(void **memory, size_t *size, char *file, size_t alignment);
