#pragma once

int ps4KernSyscallPatch(int number, sy_call_t *call, int argumentCount); //, uint32_t thrcnt)
int ps4KernSyscallPatchUnsafe(int number, sy_call_t *call, int argumentCount);
int ps4KernSyscallCopyInAndPatch(int number, sy_call_t *call, size_t size, int argumentCount); //, uint32_t thrcnt)
int ps4KernSyscallHook(int number, sy_call_t *hook);
int ps4KernSyscallUnhook(int number, sy_call_t *hook);
