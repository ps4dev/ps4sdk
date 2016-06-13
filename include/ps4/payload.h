#pragma once

#include <ps4/payload/template.h>

extern int ps4PayloadFunctionHookHandlerSize;
void ps4PayloadFunctionHookHandler();

extern int ps4PayloadLockSize;
void ps4PayloadLock();

Ps4KernelSystemCallHookArgument *ps4PayloadSystemCallHookArgumentClone(struct thread *td, Ps4KernelSystemCallHookArgument *uap, void *sysuap);
int ps4PayloadSystemCallHookHandler(struct thread *td, Ps4KernelSystemCallHookArgument *uap);

Ps4KernelFunctionHookArgument *ps4PayloadFunctionHookArgumentClone(Ps4KernelFunctionHookArgument *uap, Ps4RegisterParameters *args);
void ps4PayloadFunctionHookArgumentDestroy(Ps4KernelFunctionHookArgument *uap);
