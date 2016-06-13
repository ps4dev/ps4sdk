#define _KERNEL

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>

#include <sys/proc.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <machine/specialreg.h>
#include <machine/cpufunc.h>

#include <ps4/kernel.h>

void ps4KernelPrivilegeRoot()
{
	struct thread *td;
	struct ucred *cr;

	ps4KernelThreadGetCurrent(&td);
	cr = td->td_proc->p_ucred;
	cr->cr_uid = cr->cr_ruid = cr->cr_rgid = 0;
	//cr->cr_groups[0] = 0;
}

int ps4KernelPrivilegeUnjail()
{
	struct thread *td;
	struct filedesc	*fd;
	struct ucred *cr;
	void *t;

	ps4KernelThreadGetCurrent(&td);

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("prison0", &t));
	cr = td->td_proc->p_ucred;
	cr->cr_prison = (struct prison *)t;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("rootvnode", &t));
	fd = td->td_proc->p_fd;
	//fd->fd_cdir =
	fd->fd_rdir = fd->fd_jdir = *(struct vnode **)t;

	return PS4_OK;
}

int ps4KernelPrivilegeEscalate()
{
	ps4KernelPrivilegeRoot();
	ps4ExpressionReturnOnError(ps4KernelPrivilegeUnjail());
	return PS4_OK;
}
