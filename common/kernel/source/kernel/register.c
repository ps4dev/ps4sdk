#include <sys/cdefs.h>
#include <sys/types.h>

#include <machine/cpufunc.h>

#include <ps4/kernel.h>

int ps4KernelRegisterControlSet(unsigned int c, unsigned long value)
{
	switch(c)
	{
		case 0:
			load_cr0(value);
			break;
		/*case 1: // look into (ab)use
			load_cr1(value);
			break;*/
		/*case 2:
			load_cr2(value);
			break;*/
		case 3:
			load_cr3(value);
			break;
		case 4:
			load_cr4(value);
			break;
		default:
			return PS4_ERROR_ARGUMENT_OUT_OF_RANGE;
	}

	return PS4_OK;
}

int ps4KernelRegisterControlGet(unsigned int c, unsigned long *value)
{
	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	switch(c)
	{
		case 0:
			*value = rcr0();
			break;
		/*case 1:
			*value = rcr1();
			break;*/
		case 2:
			*value = rcr2();
			break;
		case 3:
			*value = rcr3();
			break;
		case 4:
			*value = rcr4();
			break;
		default:
			return PS4_ERROR_ARGUMENT_OUT_OF_RANGE;
	}

	return PS4_OK;
}

void ps4KernelRegisterModelSpecificSet(unsigned int msr, uint64_t value)
{
	wrmsr(msr, value);
}

int ps4KernelRegisterModelSpecificGet(unsigned int msr, uint64_t *value)
{
 	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*value = rdmsr(msr);
	return PS4_OK;
}
