#include <stdint.h>

#include <ps4/kernel.h>

int ps4KernelUartEnable()
{
	uint8_t *fn;
	uint16_t *sf;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("bootparam_disable_console_output", (void **)&fn));

	sf = (uint16_t *)(0xffffffff00000000 | (uintptr_t)*(uint32_t *)(fn + 4));
	*sf = *sf & ~(1 << 15);

	return PS4_OK;
}

int ps4KernelUartDisable()
{
	uint8_t *fn;
	uint16_t *sf;

	ps4ExpressionReturnOnError(ps4KernelSymbolLookup("bootparam_disable_console_output", (void **)&fn));

	sf = (uint16_t *)(0xffffffff00000000 | (uintptr_t)*(uint32_t *)(fn + 4));
	*sf = *sf | (1 << 15);

	return PS4_OK;
}
