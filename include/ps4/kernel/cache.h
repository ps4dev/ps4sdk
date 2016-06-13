#pragma once

typedef struct Ps4KernelCache Ps4KernelCache;

int ps4KernelCacheCreate(Ps4KernelCache **cache);
int ps4KernelCacheDestroy(Ps4KernelCache *cache);

int ps4KernelCacheGet(Ps4KernelCache *cache, const char *name, void **value);
int ps4KernelCacheSet(Ps4KernelCache *cache, const char *name, void *value);
int ps4KernelCacheDelete(Ps4KernelCache *cache, const char *name);
