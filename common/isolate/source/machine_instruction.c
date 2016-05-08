#include <stddef.h>
#include <stdint.h>

#include <ps4/kernel.h>
#include <ps4/kern.h>
#include <ps4/syscall.h>

#include <third_party/llvm/X86DisassemblerDecoder.h>

#define ps4MachineInstructionMax 1024

static int ps4MachineInstructionNextReader(const void* arg, uint8_t* byte, uint8_t *address)
{
	*byte = *address;
	return 0;
}

int ps4MachineInstructionNext(void *base, size_t *next)
{
	struct InternalInstruction internalInstr;
	uint8_t *b = (uint8_t *)base;

	if(base == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(next == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	int r = decodeInstruction(&internalInstr,
		ps4MachineInstructionNextReader, /*reader arg*/NULL,
	 	/*log*/NULL, /*logarg*/NULL, /*miiArg - NOP'ped out*/NULL,
		b + *next, MODE_64BIT);

	//*next = internalInstr.readerCursor - address;
	if(r != 0)
		return PS4_ASSEMBLER_INSTRUCTION_ERROR_NOT_DECODABLE;

	*next += internalInstr.length;

	return PS4_OK;
}

int ps4MachineInstructionSeek(void *base, size_t *offset)
{
	size_t current, n;

	if(base == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(offset == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	current = 0;
	n = *offset;
	while(current < n)
		if(ps4MachineInstructionNext(base, &current) != 0)
			return PS4_ASSEMBLER_INSTRUCTION_ERROR_NOT_DECODABLE;
	*offset = current;

	return PS4_OK;
}
