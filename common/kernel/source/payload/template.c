#include <stdint.h>

#include <ps4/kernel.h>
#include <ps4/payload.h>

//FIXME: Replace copy with fastest most atomic option(s), employ lock loop

int ps4PayloadReturnExtractValue(uint8_t *memory, int64_t *value)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

 	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturn64, memory, ps4PayloadTemplateReturn64Size) == PS4_OK)
		ps4KernelMemoryCopy(memory + 2, &value, sizeof(int64_t));
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturn32, memory, ps4PayloadTemplateReturn32Size) == PS4_OK)
		ps4KernelMemoryCopy(memory + 1, &value, sizeof(int32_t));
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturnZero, memory, ps4PayloadTemplateReturnZeroSize) == PS4_OK && value == 0)
		*value = 0;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadReturnIsPatched(uint8_t *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturn64, memory, ps4PayloadTemplateReturn64Size) == PS4_OK)
		;
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturn32, memory, ps4PayloadTemplateReturn32Size) == PS4_OK)
		;
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateReturnZero, memory, ps4PayloadTemplateReturnZeroSize) == PS4_OK)
		;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadReturnPatch(uint8_t *memory, int64_t value)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == 0)
		ps4KernelMemoryCopy((void *)ps4PayloadTemplateReturnZero, memory, ps4PayloadTemplateReturnZeroSize);
	else if(value <= INT32_MAX && value >= INT32_MIN)
	{
		ps4KernelMemoryCopy((void *)ps4PayloadTemplateReturn32, memory, ps4PayloadTemplateReturn32Size);
		ps4KernelMemoryCopy(&value, memory + 1, sizeof(int32_t));
	}
	else
	{
		ps4KernelMemoryCopy((void *)ps4PayloadTemplateReturn64, memory, ps4PayloadTemplateReturn64Size);
		ps4KernelMemoryCopy(&value, memory + 2, sizeof(int64_t));
	}

	return PS4_OK;
}

int ps4PayloadReturnDeterminePatchSize(int64_t value, size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == 0)
		*size = ps4PayloadTemplateReturnZeroSize;
	else if(value <= INT32_MAX && value >= INT32_MIN)
		*size = ps4PayloadTemplateReturn32Size;
	else
		*size = ps4PayloadTemplateReturn64Size;

	return PS4_OK;
}

int ps4PayloadJumpExtractTarget(uint8_t *memory, void **to)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(to == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*to = 0;
	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateJump32, memory, ps4PayloadTemplateJump32Size) == PS4_OK)
		ps4KernelMemoryCopy(memory + 2, to, sizeof(int32_t));
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateJump64, memory, ps4PayloadTemplateJump64Size) == PS4_OK)
		ps4KernelMemoryCopy(memory + 2, to, sizeof(void *));
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadJumpIsPatched(uint8_t *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateJump32, memory, ps4PayloadTemplateJump32Size) == PS4_OK)
		;
	else if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateJump64, memory, ps4PayloadTemplateJump64Size) == PS4_OK)
		;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadJumpPatch(uint8_t *memory, void *to)
{
	intptr_t v = (intptr_t)to;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if((v & 0xffffffff00000000) == 0)
	{
		ps4KernelMemoryCopy((void *)ps4PayloadTemplateJump32, memory, ps4PayloadTemplateJump32Size);
		ps4KernelMemoryCopy(&v, memory + 2, sizeof(int32_t));
	}
	else
	{
		ps4KernelMemoryCopy((void *)ps4PayloadTemplateJump64, memory, ps4PayloadTemplateJump64Size);
		ps4KernelMemoryCopy(&v, memory + 2, sizeof(void *));
	}

	return PS4_OK;
}

int ps4PayloadJumpDeterminePatchSize(void *to, size_t *size)
{
	intptr_t v = (intptr_t)to;

	if(size == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if((v & 0xffffffff00000000) == 0)
		*size = ps4PayloadTemplateJump32Size;
	else
		*size = ps4PayloadTemplateJump64Size;

	return PS4_OK;
}

int ps4PayloadFunctionHookPrologueExtractArgument(uint8_t *memory, Ps4KernelFunctionHookArgument **argument)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(argument == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*argument = NULL;
	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateFunctionHookPrologue, memory, ps4PayloadTemplateFunctionHookPrologueSize) == PS4_OK)
	{
		ps4KernelMemoryCopy(memory + 2, argument, sizeof(void *));
		return PS4_OK;
	}

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadFunctionHookPrologueIsPatched(uint8_t *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateFunctionHookPrologue, memory, ps4PayloadTemplateFunctionHookPrologueSize) == PS4_OK)
		return PS4_OK;

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadFunctionHookProloguePatch(uint8_t *memory, Ps4KernelFunctionHookArgument *argument)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4KernelMemoryCopy((void *)ps4PayloadTemplateFunctionHookPrologue, memory, ps4PayloadTemplateFunctionHookPrologueSize);
	ps4KernelMemoryCopy(&argument, memory + 2, sizeof(void *));

	return PS4_OK;
}

int ps4PayloadFunctionHookPrologueDeterminePatchSize(size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = ps4PayloadTemplateFunctionHookPrologueSize;
	return PS4_OK;
}

int ps4PayloadSystemCallHookPrologueExtractArgument(uint8_t *memory, Ps4KernelSystemCallHookArgument **argument)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(argument == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*argument = NULL;
	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateSystemCallHookPrologue, memory, ps4PayloadTemplateSystemCallHookPrologueSize) == PS4_OK)
	{
		ps4KernelMemoryCopy(memory + 2, argument, sizeof(void *));
		return PS4_OK;
	}

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadSystemCallHookPrologueIsPatched(uint8_t *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernelMemoryCompareNonZeros((void *)ps4PayloadTemplateSystemCallHookPrologue, memory, ps4PayloadTemplateSystemCallHookPrologueSize) == PS4_OK)
		return PS4_OK;

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadSystemCallHookProloguePatch(uint8_t *memory, Ps4KernelSystemCallHookArgument *argument)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4KernelMemoryCopy((void *)ps4PayloadTemplateSystemCallHookPrologue, memory, ps4PayloadTemplateSystemCallHookPrologueSize);
	ps4KernelMemoryCopy(&argument, memory + 2, sizeof(void *));

	return PS4_OK;
}

int ps4PayloadSystemCallHookPrologueDeterminePatchSize(size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = ps4PayloadTemplateSystemCallHookPrologueSize;
	return PS4_OK;
}

int ps4PayloadHiddenInformationExtractData(uint8_t *memory, void **data, size_t *size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	if(memory[0] != 0xeb || memory[1] > 127)
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	*size = memory[1];

	if(data != NULL)
		ps4KernelMemoryCopy(memory + 2, data, *size);

	return PS4_OK;
}

int ps4PayloadHiddenInformationIsPatched(uint8_t *memory)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(memory[0] == 0xeb && memory[1] <= 127)
		return PS4_OK;

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadHiddenInformationPatch(uint8_t *memory, void *data, uint8_t size)
{
	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;
	if(size > 127)
		return PS4_ERROR_ARGUMENT_SIZE_TOO_LARGE;

	ps4KernelMemoryFill(memory, 0xeb, 1);
	ps4KernelMemoryFill(memory + 1, size, 1);
	ps4KernelMemoryCopy(data, memory + 2, size);

	return PS4_OK;
}

int ps4PayloadHiddenInformationDeterminePatchSize(size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = 0;
	return PS4_NOT_OK;
}
