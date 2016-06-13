#include <unistd.h>
#include <sys/syscall.h>

#include <ps4/kernel.h>
#include <ps4/kernel.h>
#include <ps4/system_call.h>
#include <ps4/payload.h>
#include <ps4/exploit.h>

int ps4KernelExecute(void *fn, void *uap, int64_t *ret0, int64_t *ret1)
{
	// kernel namespace should be agnostic to kern types
	sy_call_t *f = (sy_call_t *)fn;

	if(ps4KernelIsInKernel() == PS4_OK)
	{
		struct thread *td;
		ps4KernelThreadGetCurrent(&td);
	 	int r = f(td, uap);
		if(ret0 != NULL)
			ps4KernelThreadGetReturn(td, ret0);
		if(ret1 != NULL)
			ps4KernelThreadGetSecondaryReturn(td, ret1);
		return r;
	}

	if(syscall(SYS_ps4_callback, NULL) == -1)
	{
		Ps4SystemCallGenericCallArgument u = {0};
		u.function = (void *)ps4KernelSystemCallCopyInAndPatch;
		u.rdi = SYS_ps4_callback;
		u.rsi = (register_t)ps4SystemCallGenericExecute;
		u.rdx = 128; // Ah, getting function sizes in C ... (-_-)'
		u.rcx = 4;
		ps4ExploitExecute((sy_call_t *)ps4SystemCallGenericCall, &u, NULL, NULL, NULL);
		if(syscall(SYS_ps4_callback, NULL) == -1)
		{
			//*errno = EAGAIN;
			return -1;
		}
	}

	return syscall(SYS_ps4_callback, fn, uap, ret0, ret1);
}
