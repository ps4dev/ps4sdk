#pragma once

typedef struct thread Ps4KernelThread;

int ps4KernelThreadGetCurrent(Ps4KernelThread **td);

int ps4KernelThreadGetReturn(Ps4KernelThread *td, register_t *ret);
int ps4KernelThreadGetPrimaryReturn(Ps4KernelThread *td, register_t *ret);
int ps4KernelThreadGetSecondaryReturn(Ps4KernelThread *td, register_t *ret);

int ps4KernelThreadSetReturn(Ps4KernelThread *td, register_t ret);
int ps4KernelThreadSetPrimaryReturn(Ps4KernelThread *td, register_t ret);
int ps4KernelThreadSetSecondaryReturn(Ps4KernelThread *td, register_t ret);
