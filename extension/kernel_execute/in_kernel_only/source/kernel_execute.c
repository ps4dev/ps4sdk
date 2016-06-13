#include <sys/sysent.h>
#include <ps4/error.h>

// Self-contained
static int ps4KernelExecuteIsInKernel()
{
	long long unsigned sp;
	__asm__ volatile("movq %%rsp, %0" : "=r"(sp));
	return (sp >> 48) > 0;
}

static void *ps4KernelExecuteCurrentThread()
{
	struct thread *td;
	__asm__ volatile("movq %%gs:0, %0;" : "=r"(td));
	return td;
}

static inline register_t ps4KernelExecuteThreadGetReturn0(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x380]));
}

static inline register_t ps4KernelExecuteThreadGetReturn1(struct thread *td)
{
	return *((register_t *)&(((int8_t *)td)[0x388]));
}

int ps4KernelExecute(void *fn, void *uap, int64_t *ret0, int64_t *ret1)
{
	sy_call_t *f = (sy_call_t *)fn;

	if(ps4KernelExecuteIsInKernel() == PS4_OK)
	{
		void *td = ps4KernelExecuteCurrentThread();
	 	int r = f(td, uap);
		if(ret0 != NULL)
			*ret0 = ps4KernelExecuteThreadGetReturn0(td);
		if(ret1 != NULL)
			*ret1 = ps4KernelExecuteThreadGetReturn1(td);
		return r;
	}

	return PS4_ERROR_NOT_IMPLEMENTED;
}
