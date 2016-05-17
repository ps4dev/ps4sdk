#pragma once

#include <stdlib.h>
#include <stdio.h>

void ps4UtilStandardIoRedirect(int to, int stdfd[3], fpos_t stdpos[3]);
void ps4UtilStandardIoRedirectReset(int stdfd[3], fpos_t stdpos[3]);

void ps4UtilStandardIoRedirectPlain(int to);

int ps4UtilServerCreateEx(int port, int backlog, int try, unsigned int sec);
int ps4UtilServerCreate(int port);
int ps4UtilServerCreateSingleAccept(int port);

FILE *ps4UtilFileOpenDescriptorDuplicate(int fd, const char *mode); //fddupopen

void *ps4UtilFileAllocateFromDescriptorUnsized(int fd, size_t *size);
void *ps4UtilFileAllocateFromPathAligned(char *file, size_t *size, size_t alignment);
#define ps4UtilFileAllocateFromPath(file, size) ps4UtilFileAllocateFromPathAligned(file, size, 1)
