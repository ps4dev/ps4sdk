#include <stddef.h>
#include <stdint.h>

#include <ps4/kernel.h>

#include <third_party/udis86/udis86.h>

int ps4MachineInstructionNext(void *current, void **next)
{
    ud_t ud_obj;

	if(current == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(next == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

    ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, current, (size_t)-1);
    ud_set_mode(&ud_obj, 64);
    ud_set_syntax(&ud_obj, UD_SYN_ATT);

	if(!ud_disassemble(&ud_obj))
		return PS4_ERROR_ASSEMBLER_INSTRUCTION_NOT_DECODABLE;

	*next = (uint8_t *)current + ud_insn_len(&ud_obj);

	return PS4_OK;
}

int ps4MachineInstructionSeek(void *current, void **found, size_t offset)
{
	ud_t ud_obj;
	size_t i;

	if(current == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(found == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

    ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, current, (size_t)-1);
    ud_set_mode(&ud_obj, 64);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	i = 0;
	while(i < offset && ud_disassemble(&ud_obj))
		i += ud_insn_len(&ud_obj);

	if(i == 0 && offset != 0)
		return PS4_ERROR_ASSEMBLER_INSTRUCTION_NOT_DECODABLE;

	current = (uint8_t *)current + i;

	*found = current;

	return PS4_OK;
}
