#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>

#include <machine/specialreg.h>
#include <machine/cpufunc.h>

#include <ps4/kernel.h>

static uint64_t *pg_nx;

void ps4KernelProtectionWriteDisable()
{
	load_cr0(rcr0() & ~CR0_WP);
}

void ps4KernelProtectionWriteEnable()
{
	load_cr0(rcr0() | CR0_WP);
}

void ps4KernelProtectionExecuteDisable() // gives you that extra tingly feeling (TM)
{
	if(pg_nx == NULL)
		pg_nx = (uint64_t *)ps4KernelDlSym("pg_nx");
	*pg_nx = 0;
}

void ps4KernelProtectionExecuteEnable()
{
	if(pg_nx == NULL)
		pg_nx = (uint64_t *)ps4KernelDlSym("pg_nx");
	*pg_nx = 1ull << 63;
}

void ps4KernelProtectionAllDisable()
{
	ps4KernelProtectionWriteDisable();
	ps4KernelProtectionExecuteDisable();
}

void ps4KernelProtectionAllEnable()
{
	ps4KernelProtectionExecuteEnable();
	ps4KernelProtectionWriteEnable();
}
