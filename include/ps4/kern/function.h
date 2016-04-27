#pragma once

void ps4KernFunctionPatchToTruth(void *function);
int ps4KernFunctionPatchToJumpTo(void *function, void *to);
void ps4KernFunctionPatchToJumpToUnsafe(void *function, void *to);
int ps4KernFunctionHook(void *function, void *hook);
int ps4KernFunctionUnhook(void *function, void *hook);
