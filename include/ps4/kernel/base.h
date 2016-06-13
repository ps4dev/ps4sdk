#pragma once

// base
//{
void *ps4KernelSeekElfAddress();
void *ps4KernelDlSym(char *name);
//}

int ps4KernelBrewCoffee();

int ps4KernelIsKernelAddress(void *address);
int ps4KernelIsInKernel();

extern int ps4KernelExecute(void *fn, void *uap, int64_t *ret0, int64_t *ret1);
int64_t ps4KernelCall();
