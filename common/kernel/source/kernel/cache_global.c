#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/sysctl.h>

#include <sys/lock.h>
#include <sys/mutex.h>

#include <ps4/kernel.h>

typedef int Ps4KernelCacheGlobalHandlerUnsignedInteger64(struct sysctl_oid *, void *, intptr_t, struct sysctl_req *);

static Ps4KernelCache *cache;
static struct mtx *ps4KernelCacheGlobalMutex; // FIXME: Use RW lock or RM locks

#define PS4_KERNEL_CACHE_GLOBAL "ps4.kernel.cache.global"
#define PS4_KERNEL_CACHE_GLOBAL_MUTEX PS4_KERNEL_CACHE_GLOBAL ".mutex"

/* bsd */
static int name2oid(char *name, int *oid, int *len, struct sysctl_oid **oidpp)
{
	int i;
	struct sysctl_oid *oidp;
	struct sysctl_oid_list *lsp = (struct sysctl_oid_list *)ps4KernelDlSym("sysctl__children");
	char *p;

	if (!*name)
		return (ENOENT);

	p = name + strlen(name) - 1 ;
	if (*p == '.')
		*p = '\0';

	*len = 0;

	for (p = name; *p && *p != '.'; p++)
		;
	i = *p;
	if (i == '.')
		*p = '\0';

	oidp = SLIST_FIRST(lsp);

	while (oidp && *len < CTL_MAXNAME) {
		if (strcmp(name, oidp->oid_name)) {
			oidp = SLIST_NEXT(oidp, oid_link);
			continue;
		}
		*oid++ = oidp->oid_number;
		(*len)++;

		if (!i) {
			if (oidpp)
				*oidpp = oidp;
			return (0);
		}

		if ((oidp->oid_kind & CTLTYPE) != CTLTYPE_NODE)
			break;

		if (oidp->oid_handler)
			break;

		lsp = SYSCTL_CHILDREN(oidp);
		oidp = SLIST_FIRST(lsp);
		name = p+1;
		for (p = name; *p && *p != '.'; p++)
				;
		i = *p;
		if (i == '.')
			*p = '\0';
	}
	return (ENOENT);
}

int ps4KernelCacheGlobalInitialize()
{
	Ps4KernelThread *td;
	struct mtx *giant;
	struct sysctl_oid *cacheid;
	size_t size;
	uint64_t *c;
	int r;

	giant = ps4KernelDlSym("Giant");

	r = ps4KernelThreadGetCurrent(&td);
	if(r != PS4_OK)
		return r;

	mtx_lock(giant);
	cacheid = NULL;
	size = sizeof(Ps4KernelCache *);
	r = kernel_sysctlbyname(td, "ps4.cache", &cache, &size, NULL, 0, NULL, 0);
	if(r == 0)
	{
		ps4KernelCacheGet(cache, PS4_KERNEL_CACHE_GLOBAL_MUTEX, (void **)&ps4KernelCacheGlobalMutex);
		mtx_unlock(giant);
		return PS4_OK;
	}
	else if(r != 0)
	{
		r = ps4KernelCacheCreate(&cache);
		if(r != PS4_OK)
		{
			mtx_unlock(giant);
			return r;
		}
		ps4KernelMemoryAllocateData((void **)&c, 8);
		if(c == NULL)
		{
			ps4KernelCacheDestroy(cache);
			mtx_unlock(giant);
			return PS4_ERROR_KERNEL_OUT_OF_MEMORY;
		}
		*c = (uint64_t)cache;

		ps4KernelMemoryAllocateData((void **)&ps4KernelCacheGlobalMutex, sizeof(struct mtx));
		if(ps4KernelCacheGlobalMutex == NULL)
		{
			ps4KernelMemoryFree(c);
			ps4KernelCacheDestroy(cache);
			mtx_unlock(giant);
			return PS4_ERROR_KERNEL_OUT_OF_MEMORY;
		}
		mtx_init(ps4KernelCacheGlobalMutex, "ps4KernelCacheGlobalMutex", NULL, MTX_DEF | MTX_RECURSE);
		ps4KernelCacheSet(cache, PS4_KERNEL_CACHE_GLOBAL_MUTEX, ps4KernelCacheGlobalMutex);

		size = sizeof(Ps4KernelCache *);
		r = kernel_sysctlbyname(td, "ps4", NULL, NULL, NULL, 0, NULL, 0);
		if(r == ENOENT)
		{
			char *fmt1, *fmt2;
			ps4KernelMemoryAllocateData((void **)&fmt1, 2);
			ps4KernelMemoryAllocateData((void **)&fmt2, 3);
			strcpy(fmt1, "N");
			strcpy(fmt2, "QU");

			struct sysctl_oid_list *root = (struct sysctl_oid_list *)ps4KernelDlSym("sysctl__children");
			struct sysctl_oid *ps4 = sysctl_add_oid(NULL, root, OID_AUTO, "ps4", CTLTYPE_NODE | CTLFLAG_RW, NULL, 0, 0, fmt1, "ps4sdk system information");
			cacheid = sysctl_add_oid(NULL, SYSCTL_CHILDREN(ps4), OID_AUTO, "cache", CTLTYPE_U64 | CTLFLAG_MPSAFE | CTLFLAG_RW, c, 0, (Ps4KernelCacheGlobalHandlerUnsignedInteger64 *)ps4KernelDlSym("sysctl_handle_64"), fmt2, "ps4 global cache");
		}
		else if(r == EISDIR)
		{
			int oid[CTL_MAXNAME], len;
			struct sysctl_oid *ps4;
			char *fmt2;
			ps4KernelMemoryAllocateData((void **)&fmt2, 3);
			strcpy(fmt2, "QU");

			len = 0;
			ps4 = NULL;
			r = name2oid("ps4", oid, &len, &ps4);
			cacheid = sysctl_add_oid(NULL, SYSCTL_CHILDREN(ps4), OID_AUTO, "cache", CTLTYPE_U64 | CTLFLAG_MPSAFE | CTLFLAG_RW, c, 0, (Ps4KernelCacheGlobalHandlerUnsignedInteger64 *)ps4KernelDlSym("sysctl_handle_64"), fmt2, "ps4 global cache");
		}
		else
		{
			ps4KernelMemoryFree(ps4KernelCacheGlobalMutex);
			ps4KernelMemoryFree(c);
			ps4KernelCacheDestroy(cache);
			mtx_unlock(giant);
			return PS4_ERROR_INTERNAL_ERROR;
		}
	}

	mtx_unlock(giant);
	if(cacheid == NULL)
		return PS4_NOT_OK;
	return PS4_OK;
}

int ps4KernelCacheGlobalGet(char *name, void **value)
{
	int r;

	if(cache == NULL)
		ps4ExpressionReturnOnError(ps4KernelCacheGlobalInitialize());

	if(name == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	mtx_lock(ps4KernelCacheGlobalMutex);
	r = ps4KernelCacheGet(cache, name, value);
	mtx_unlock(ps4KernelCacheGlobalMutex);

	return r;
}

int ps4KernelCacheGlobalSet(char *name, void *value)
{
	int r;

	if(cache == NULL)
		ps4ExpressionReturnOnError(ps4KernelCacheGlobalInitialize());

	if(name == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(value == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	mtx_lock(ps4KernelCacheGlobalMutex);
 	r = ps4KernelCacheSet(cache, name, value);
	mtx_unlock(ps4KernelCacheGlobalMutex);

	return r;
}

int ps4KernelCacheGlobalDelete(char *name)
{
	int r;

	if(cache == NULL)
		ps4ExpressionReturnOnError(ps4KernelCacheGlobalInitialize());

	if(name == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	mtx_lock(ps4KernelCacheGlobalMutex);
	r = ps4KernelCacheDelete(cache, name);
	mtx_unlock(ps4KernelCacheGlobalMutex);

	return r;
}
