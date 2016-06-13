#pragma once

#include <stdint.h>

int ps4KernelStubResolve(void *stub, char *functionName, void **kernelAddress);
int ps4StubResolve(void *stub, char *moduleName, char *functionName, int *moduleHandle, void **userAddress);
int ps4AdaptiveStubResolve(void *stub, char *moduleName, char *functionName, int *moduleHandle, void **userAddress, void **kernelAddress);
