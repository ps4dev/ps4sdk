#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/malloc.h>

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <bsm/audit_kevents.h>

#include <elf.h>
#include <machine/specialreg.h>

#include <ps4/kern.h>

static uint8_t *ps4KernMemoryBaseAddress;
static uint8_t *ps4KernMemoryCurrentAddress;

//FIXME: Find way to malloc RWX memory ... not safe at all (>.<)
int ps4KernMemoryAllocate(void **memory, size_t size)
{
	struct mtx *mutex;
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCurrentAddress == NULL)
	{
		mutex = ps4KernDlSym("Giant");
		void **base = (void **)ps4HackKernMemoryGetBaseAddress();

		mtx_lock(mutex);
		ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
		// make sure we do not override prior mallocs until reboot
		// [CACHE][REGISTY][MTXNAME][MUTEX][MAGIC][FREEPOINTER][FREE ...]
		if(base[4] != (void *)0x0bad0d060acce550)
		{
			struct malloc_type *mt = ps4KernDlSym("M_TEMP");
			base[0] = NULL;
			base[1] = NULL;
			base[2] = malloc(8, mt, M_ZERO | M_WAITOK);
			char *name = base[2];
			name[0] = 'm'; name[1] = 'e'; name[2] = 'm'; name[3] = '\0';
			base[3] = malloc(sizeof(struct mtx), mt, M_ZERO | M_WAITOK);
			mtx_init(base[3], name, NULL, MTX_DEF | MTX_RECURSE | MTX_QUIET);// | MTX_NOWITNESS | MTX_DUPOK | MTX_NOPROFILE);
			base[4] = (void *)0x0bad0d060acce550;
			base[5] = &base[6];
		}
		ps4KernMemoryBaseAddress = (uint8_t *)base;
		ps4KernMemoryCurrentAddress = base[5];
		ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);
		mtx_unlock(mutex);
	}

	mutex = (struct mtx *)(((void **)ps4KernMemoryBaseAddress)[3]);
	mtx_lock(mutex);
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
	*memory = ps4KernMemoryCurrentAddress;
	ps4KernMemoryCurrentAddress += size;
	((void **)ps4KernMemoryBaseAddress)[5] = ps4KernMemoryCurrentAddress;
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);
	mtx_unlock(mutex);

	return PS4_OK;
}

void *ps4KernMemoryMalloc(size_t size)
{
	void *m;
	if(ps4KernMemoryAllocate(&m, size) != 0)
		return NULL;
	return m;
}

void ps4KernMemoryFree(void *memory)
{

}

int ps4KernMemoryCopy(void *from, void *to, size_t size)
{
	if(from == NULL || to == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
	memcpy(to, from, size);
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);

	return PS4_OK;
}

int ps4KernMemorySwap(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
	for(size_t i = 0; i < size; ++i)
	{
		uint8_t t = at[i];
		at[i] = bt[i];
		bt[i] = t;
	}
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);

	return PS4_OK;
}

int ps4KernMemoryCompareNonZeros(void *a, void *b, size_t size)
{
	uint8_t *at = a, *bt = b;

	if(a == NULL || b == NULL)
		return PS4_ERROR_ARGUMENT_MISSING;

	for(size_t i = 0; i < size; ++i)
		if(at[i] != 0 && at[i] != bt[i])
			return at[i] - bt[i];

	return PS4_OK;
}
