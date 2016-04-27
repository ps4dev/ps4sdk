#pragma once

void ps4KernRegisterCr0Set(register_t cr0); // load_cr0
register_t ps4KernRegisterCr0Get(void); // rcr0

struct thread *ps4KernThreadCurrent(void);
register_t ps4KernThreadGetReturn0(struct thread *td);
register_t ps4KernThreadGetReturn1(struct thread *td);
void ps4KernThreadSetReturn0(struct thread *td, register_t ret);
void ps4KernThreadSetReturn1(struct thread *td, register_t ret);

int ps4KernIsKernelAddress(void *address);
int ps4KernIsInKernel();

void *ps4KernSeekElfAddress();
void *ps4KernDlSym(char *symbol);
