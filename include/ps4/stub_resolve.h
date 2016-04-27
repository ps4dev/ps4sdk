#pragma once

#include <stdint.h>

int ps4KernStubResolve(void *stubAddress, char *functionName, void **kernelAddress);
int ps4StubResolve(void *stubAddress, char *moduleName, char *functionName, int *moduleHandle, void **userAddress);
int ps4MixedStubResolve(void *stubAddress, char *moduleName, char *functionName, int *moduleHandle, void **userAddress, void **kernelAddress);
