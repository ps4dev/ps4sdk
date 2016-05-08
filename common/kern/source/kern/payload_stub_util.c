#define _KERNEL

#include <stdint.h>

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/libkern.h>

#include <sys/kthread.h>
#include <sys/filedesc.h>
#include <sys/ucred.h>

#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/malloc.h>

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <bsm/audit_kevents.h>

#include <sys/errno.h>

#include <elf.h>
#include <machine/specialreg.h>

#include <ps4/kern.h>
#include <ps4/payload.h>

//FIXME: Replace copy with fastest most atomic option(s)

int ps4PayloadStubUtilReturnGet(uint8_t *stub, register_t *value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

 	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn64, stub, ps4PayloadStubReturn64Size) == PS4_OK)
		ps4KernMemoryCopy(stub + 2, &value, sizeof(register_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn32, stub, ps4PayloadStubReturn32Size) == PS4_OK)
		ps4KernMemoryCopy(stub + 1, &value, sizeof(int32_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturnZero, stub, ps4PayloadStubReturnZeroSize) == PS4_OK && value == 0)
		*value = 0;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilReturnSet(uint8_t *stub, register_t value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn64, stub, ps4PayloadStubReturn64Size) == PS4_OK)
		ps4KernMemoryCopy(&value, stub + 2, sizeof(register_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn32, stub, ps4PayloadStubReturn32Size)  == PS4_OK && value <= INT32_MAX && value >= INT32_MIN)
		ps4KernMemoryCopy(&value, stub + 1, sizeof(int32_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturnZero, stub, ps4PayloadStubReturnZeroSize)  == PS4_OK && value == 0)
		; // set null to null
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilReturnIs(uint8_t *stub)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn64, stub, ps4PayloadStubReturn64Size) == PS4_OK)
		;
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturn32, stub, ps4PayloadStubReturn32Size) == PS4_OK)
		;
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubReturnZero, stub, ps4PayloadStubReturnZeroSize) == PS4_OK)
		;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilReturnPatch(uint8_t *stub, register_t value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == 0)
		ps4KernMemoryCopy((void *)ps4PayloadStubReturnZero, stub, ps4PayloadStubReturnZeroSize);
	else if(value <= INT32_MAX && value >= INT32_MIN)
	{
		ps4KernMemoryCopy((void *)ps4PayloadStubReturn32, stub, ps4PayloadStubReturn32Size);
		ps4KernMemoryCopy(&value, stub + 1, sizeof(int32_t));
	}
	else
	{
		ps4KernMemoryCopy((void *)ps4PayloadStubReturn64, stub, ps4PayloadStubReturn64Size);
		ps4KernMemoryCopy(&value, stub + 2, sizeof(register_t));
	}

	return PS4_OK;
}

int ps4PayloadStubUtilReturnSize(register_t value, size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == 0)
		*size = ps4PayloadStubReturnZeroSize;
	else if(value <= INT32_MAX && value >= INT32_MIN)
		*size = ps4PayloadStubReturn32Size;
	else
		*size = ps4PayloadStubReturn64Size;

	return PS4_OK;
}

int ps4PayloadStubUtilJumpGet(uint8_t *stub, register_t *value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*value = 0;
	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump32, stub, ps4PayloadStubJump32Size) == PS4_OK)
		ps4KernMemoryCopy(stub + 2, value, sizeof(int32_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump64, stub, ps4PayloadStubJump64Size) == PS4_OK)
		ps4KernMemoryCopy(stub + 2, value, sizeof(register_t));
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilJumpSet(uint8_t *stub, register_t value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if((value & 0xffffffff00000000) == 0 && ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump32, stub, ps4PayloadStubJump32Size) == PS4_OK)
		ps4KernMemoryCopy(&value, stub + 2, sizeof(int32_t));
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump64, stub, ps4PayloadStubJump64Size) == PS4_OK)
		ps4KernMemoryCopy(&value, stub + 2, sizeof(register_t));
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilJumpIs(uint8_t *stub)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump32, stub, ps4PayloadStubJump32Size) == PS4_OK)
		;
	else if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJump64, stub, ps4PayloadStubJump64Size) == PS4_OK)
		;
	else
		return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;

	return PS4_OK;
}

int ps4PayloadStubUtilJumpPatch(uint8_t *stub, register_t value)
{
	intptr_t v = (intptr_t)value;

	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if((v & 0xffffffff00000000) == 0)
	{
		ps4KernMemoryCopy((void *)ps4PayloadStubJump32, stub, ps4PayloadStubJump32Size);
		ps4KernMemoryCopy(&v, stub + 2, sizeof(int32_t));
	}
	else
	{
		ps4KernMemoryCopy((void *)ps4PayloadStubJump64, stub, ps4PayloadStubJump64Size);
		ps4KernMemoryCopy(&v, stub + 2, sizeof(register_t));
	}

	return PS4_OK;
}

int ps4PayloadStubUtilJumpSize(register_t value, size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if((value & 0xffffffff00000000) == 0)
		*size = ps4PayloadStubJump32Size;
	else
		*size = ps4PayloadStubJump64Size;

	return PS4_OK;
}

int ps4PayloadStubUtilJumpHookStage0Get(uint8_t *stub, register_t *value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(value == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;

	*value = 0;
	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJumpHookStage0, stub, ps4PayloadStubJumpHookStage0Size) == PS4_OK)
	{
		ps4KernMemoryCopy(stub + 2, value, sizeof(register_t));
		return PS4_OK;
	}

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadStubUtilJumpHookStage0Set(uint8_t *stub, register_t value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJumpHookStage0, stub, ps4PayloadStubJumpHookStage0Size) == PS4_OK)
	{
		ps4KernMemoryCopy(&value, stub + 2, sizeof(register_t));
		return PS4_OK;
	}

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadStubUtilJumpHookStage0Size(register_t value, size_t *size)
{
	if(size == NULL)
		return PS4_ERROR_ARGUMENT_OUT_MISSING;
	*size = ps4PayloadStubJumpHookStage0Size;
	return PS4_OK;
}

int ps4PayloadStubUtilJumpHookStage0Is(uint8_t *stub)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	if(ps4KernMemoryCompareNonZeros((void *)ps4PayloadStubJumpHookStage0, stub, ps4PayloadStubJumpHookStage0Size) == PS4_OK)
		return PS4_OK;

	return PS4_ERROR_ARGUMENT_EXPECTATION_FAILED;
}

int ps4PayloadStubUtilJumpHookStage0Patch(uint8_t *stub, register_t value)
{
	if(stub == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4KernMemoryCopy((void *)ps4PayloadStubJumpHookStage0, stub, ps4PayloadStubJumpHookStage0Size);
	ps4KernMemoryCopy(&value, stub + 2, sizeof(register_t));

	return PS4_OK;
}
