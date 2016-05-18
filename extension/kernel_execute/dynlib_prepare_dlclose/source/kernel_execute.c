#include <unistd.h>
#include <sys/syscall.h>

#include <ps4/kern.h>
#include <ps4/kernel.h>
#include <ps4/syscall.h>
#include <ps4/payload.h>
#include <ps4/exploit.h>

int ps4KernelExecute(void *fn, void *uap, int64_t *ret0, int64_t *ret1)
{
	// kernel namespace should be agnostic to kern types
	sy_call_t *f = (sy_call_t *)fn;

	if(ps4KernelIsInKernel())
	{
		void *td = ps4KernThreadCurrent();
	 	int r = f(td, uap);
		if(ret0 != NULL)
			*ret0 = ps4KernThreadGetReturn0(td);
		if(ret1 != NULL)
			*ret1 = ps4KernThreadGetReturn1(td);
		return r;
	}

	if(syscall(SYS_ps4_callback, NULL) == -1)
	{
		Ps4SyscallGenericCallArgument u = {0};
		u.function = (void *)ps4KernSyscallCopyInAndPatch;
		u.rdi = SYS_ps4_callback;
		u.rsi = (register_t)ps4SyscallGenericExecute;
		u.rdx = 128; // Ah, getting function sizes in C ... (-_-)'
		u.rcx = 4;
		ps4ExploitExecute((sy_call_t *)ps4SyscallGenericCall, &u, NULL, NULL, NULL);
		if(syscall(SYS_ps4_callback, NULL) == -1)
		{
			//*errno = EAGAIN;
			return -1;
		}
	}

	return syscall(SYS_ps4_callback, fn, uap, ret0, ret1);
}
