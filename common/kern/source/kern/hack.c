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

// Hack namespace means: used only til we put more effort into it
// Code can be extremely dirty to poc something faster
// Build stuff minimalistic and replaceable
typedef struct Ps4HackKernGlobalRegistry
{
	register_t *table;
	register_t size;
}
Ps4HackKernGlobalRegistry;

static uint8_t *ps4HackKernMemoryBaseAddress;

uint8_t *ps4HackKernMemoryGetBaseAddress()
{
	if(ps4HackKernMemoryBaseAddress == NULL)
	{
		void *kernelAddress = ps4KernSeekElfAddress();
		if(kernelAddress == NULL)
			return NULL;

		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)kernelAddress;
		Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ehdr + ehdr->e_phoff);
		for(size_t i = 0; i < ehdr->e_phnum; i++, phdr++)
			if (phdr->p_type == PT_LOAD)
			{
				ps4HackKernMemoryBaseAddress = (uint8_t *)phdr->p_vaddr;
				break;
			}
	}

	return ps4HackKernMemoryBaseAddress;
}
