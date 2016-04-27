#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>
#include <bsm/audit_kevents.h>

#include <sys/syscall.h>
#include <sys/sysent.h>

#include <stdint.h>

#include <ps4/kern.h>

int ps4KernUartEnable()
{
	uint8_t *fn;
	uint16_t *sf;
	fn = (uint8_t *)ps4KernDlSym("bootparam_disable_console_output");
	if(fn == NULL)
		return -1;
	sf = (uint16_t *)(0xffffffff00000000 | (uintptr_t)*(uint32_t *)(fn + 4));
	*sf = *sf & ~(1 << 15);
	return 0;
}

int ps4KernUartDisable()
{
	uint8_t *fn;
	uint16_t *sf;
	fn = (uint8_t *)ps4KernDlSym("bootparam_disable_console_output");
	if(fn == NULL)
		return -1;
	sf = (uint16_t *)(0xffffffff00000000 | (uintptr_t)*(uint32_t *)(fn + 4));
	*sf = *sf | (1 << 15);
	return 0;
}
