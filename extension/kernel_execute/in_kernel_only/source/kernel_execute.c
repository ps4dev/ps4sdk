#include <sys/sysent.h>
#include <ps4/error.h>

// Self-contained
static int ps4KernelExecuteEarlyIsInKernel()
{
	long long unsigned sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return (sp >> 48) > 0;
}

static void *ps4KernelExecuteEarlyCurrentThread()
{
	struct thread *td;
	__asm__ volatile("movq %%gs:0, %0;" : "=r"(td));
	return td;
}

int ps4KernelExecute(sy_call_t *fn, void *uap)
{
	if(ps4KernelExecuteEarlyIsInKernel())
		return fn(ps4KernelExecuteEarlyCurrentThread(), uap);
	return PS4_ERROR_NOT_IMPLEMENTED;
}
