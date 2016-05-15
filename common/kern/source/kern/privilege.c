#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/proc.h>
#include <sys/event.h>
#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <ps4/kern.h>

#include <machine/specialreg.h>
#include <machine/cpufunc.h>

void ps4KernProtectionWriteDisable()
{
	load_cr0(rcr0() & ~CR0_WP);
}

void ps4KernProtectionWriteEnable()
{
	load_cr0(rcr0() | CR0_WP);
}

void ps4KernProtectionExecuteDisable() // gives you that extra tingly feeling (TM)
{
	uint64_t *ptr = (uint64_t *)ps4KernDlSym("pg_nx");
	*ptr = 0;
}

void ps4KernProtectionExecuteEnable()
{
	uint64_t *ptr = (uint64_t *)ps4KernDlSym("pg_nx");
	*ptr = 1ull << 63;
}

void ps4KernProtectionAllDisable()
{
	ps4KernProtectionWriteDisable();
	ps4KernProtectionExecuteDisable();
}

void ps4KernProtectionAllEnable()
{
	ps4KernProtectionExecuteEnable();
	ps4KernProtectionWriteEnable();
}

void ps4KernPrivilegeRoot()
{
	struct thread *td;
	struct ucred *cr;

	td = ps4KernThreadCurrent();
	cr = td->td_proc->p_ucred;
	cr->cr_uid = cr->cr_ruid = cr->cr_rgid = 0;
	//cr->cr_groups[0] = 0;
}

int ps4KernPrivilegeUnjail()
{
	struct thread *td;
	struct filedesc	*fd;
	struct ucred *cr;
	void *t;

	td = ps4KernThreadCurrent();

	t = ps4KernDlSym("prison0");
	if(t == NULL)
		return PS4_KERN_ERROR_DLSYM_NOT_FOUND;
	cr = td->td_proc->p_ucred;
	cr->cr_prison = (struct prison *)t;

	t = ps4KernDlSym("rootvnode");
	if(t == NULL)
		return PS4_KERN_ERROR_DLSYM_NOT_FOUND;
	fd = td->td_proc->p_fd;
	//fd->fd_cdir =
	fd->fd_rdir = fd->fd_jdir = *(struct vnode **)t;

	return PS4_OK;
}

int ps4KernPrivilegeEscalate()
{
	ps4KernPrivilegeRoot();
	if(ps4KernPrivilegeUnjail() != PS4_OK)
		return PS4_ERROR_INTERNAL_ERROR;
	return PS4_OK;
}
