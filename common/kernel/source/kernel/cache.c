#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>

#include <sys/sysctl.h>

#include <ps4/kernel.h>
#include <ps4/error.h>
#include <ps4/limit.h>

#include <third_party/kernel/uthash/uthash.h>

typedef struct Ps4KernelCacheEntry
{
    const char *name;
    void *value;
    UT_hash_handle hh;
}
Ps4KernelCacheEntry;

typedef struct Ps4KernelCache
{
    Ps4KernelCacheEntry *handle;
}
Ps4KernelCache;

int ps4KernelCacheCreate(Ps4KernelCache **cache)
{
	int r;

	if(cache == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	r = ps4KernelMemoryAllocateData((void **)cache, sizeof(Ps4KernelCache));
	if(r != PS4_OK)
		return r;

	(*cache)->handle = NULL;

	return PS4_OK;
}

int ps4KernelCacheDestroy(Ps4KernelCache *cache)
{
	Ps4KernelCacheEntry *entry, *t;

	if(cache == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	HASH_ITER(hh, cache->handle, entry, t)
	{
		HASH_DEL(cache->handle, entry);
		ps4KernelMemoryFree(entry);
	}
 	ps4KernelMemoryFree(cache);

	return PS4_OK;
}

int ps4KernelCacheGet(Ps4KernelCache *cache, const char *name, void **value)
{
	Ps4KernelCacheEntry *entry;

	if(cache == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*value = NULL;
    entry = NULL;

	HASH_FIND(hh, cache->handle, name, strnlen(name, PS4_LIMIT_STRING_LENGTH_MAXIMUM), entry);
	if(entry == NULL)
		return PS4_ERROR_NOT_FOUND;
	*value = entry->value;

	return PS4_OK;
}

int ps4KernelCacheSet(Ps4KernelCache *cache, const char *name, void *value)
{
	Ps4KernelCacheEntry *entry;
	size_t length;

	if(cache == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	length = strnlen(name, PS4_LIMIT_STRING_LENGTH_MAXIMUM);
	entry = NULL;

	HASH_FIND(hh, cache->handle, name, length, entry);
	if(entry != NULL)
	{
		entry->value = value;
		return PS4_OK;
	}
	ps4KernelMemoryAllocateData((void **)&entry, sizeof(Ps4KernelCacheEntry));
	ps4KernelMemoryAllocateData((void **)&entry->name, length + 1);
	memcpy((void *)entry->name, name, length + 1);
	entry->value = value;
	HASH_ADD_KEYPTR(hh, cache->handle, entry->name, length, entry);

	return PS4_OK;

}

int ps4KernelCacheDelete(Ps4KernelCache *cache, const char *name)
{
	Ps4KernelCacheEntry *entry;

	if(cache == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	entry = NULL;

	HASH_FIND(hh, cache->handle, name, strnlen(name, PS4_LIMIT_STRING_LENGTH_MAXIMUM), entry);
	if(entry == NULL)
		return PS4_ERROR_NOT_FOUND;
	HASH_DEL(cache->handle, entry);
	ps4KernelMemoryFree(entry);

	return PS4_OK;
}
