#pragma once

#include <ps4/kernel/thread.h>

int ps4KernelCacheGlobalGet(char *name, void **value);
int ps4KernelCacheGlobalSet(char *name, void *value);
int ps4KernelCacheGlobalDelete(char *name);
