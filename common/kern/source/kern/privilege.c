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
