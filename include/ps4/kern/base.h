#pragma once

void ps4KernRegisterCr0Set(register_t cr0);
register_t ps4KernRegisterCr0Get(void);
void ps4KernRegisterMsrSet(register_t msr, register_t value);
register_t ps4KernRegisterMsrGet(register_t msr);

int ps4KernBrewCoffee();

struct thread *ps4KernThreadCurrent(void);
register_t ps4KernThreadGetReturn0(struct thread *td);
register_t ps4KernThreadGetReturn1(struct thread *td);
void ps4KernThreadSetReturn0(struct thread *td, register_t ret);
void ps4KernThreadSetReturn1(struct thread *td, register_t ret);

int ps4KernIsKernelAddress(void *address);
int ps4KernIsInKernel();

void *ps4KernSeekElfAddress();
void *ps4KernDlSym(char *symbol);
