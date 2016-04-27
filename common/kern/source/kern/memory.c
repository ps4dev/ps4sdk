#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <bsm/audit_kevents.h>

#include <elf.h>
#include <machine/specialreg.h>

#include <ps4/kern.h>

static uint8_t *ps4KernelMemoryBaseAddress;
static uint8_t *ps4KernelMemoryCurrentAddress;

//FIXME: Find way to malloc exec memory ... not safe at all (>.<)
int ps4KernMemoryAllocate(void **memory, size_t size)
{
	if(memory == NULL)
		return -1;

	if(ps4KernelMemoryCurrentAddress == NULL)
	{
		void *kernelAddress = ps4KernSeekElfAddress();
		if(kernelAddress == NULL)
			return -2;

		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)kernelAddress;
		Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ehdr + ehdr->e_phoff);
		for(size_t i = 0; i < ehdr->e_phnum; i++, phdr++)
			if (phdr->p_type == PT_LOAD)
			{
				ps4KernelMemoryBaseAddress = (uint8_t *)phdr->p_vaddr;
				break;
			}

		if(ps4KernelMemoryBaseAddress == NULL)
			return -2;

		// make sure we do not override prior mallocs until reboot
		// (magic, current) at start of alloc space
		if(*(int64_t *)ps4KernelMemoryBaseAddress != 0x0bad0d060acce550)
		{
			ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
			*(int64_t *)ps4KernelMemoryBaseAddress = 0x0bad0d060acce550;
			*(int64_t *)(ps4KernelMemoryBaseAddress + 8) = (int64_t)(ps4KernelMemoryBaseAddress + 16);
			ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);
		}

		ps4KernelMemoryBaseAddress += 8;
		ps4KernelMemoryCurrentAddress = (uint8_t *)*(int64_t *)ps4KernelMemoryBaseAddress;
	}

	*memory = ps4KernelMemoryCurrentAddress;
	ps4KernelMemoryCurrentAddress += size;
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
	*(int64_t *)ps4KernelMemoryBaseAddress = (int64_t)ps4KernelMemoryCurrentAddress;
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);

	return 0;
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

void ps4KernMemoryCopy(void *from, void *to, size_t size)
{
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() & ~CR0_WP);
	memcpy(to, from, size);
	ps4KernRegisterCr0Set(ps4KernRegisterCr0Get() | CR0_WP);
}
