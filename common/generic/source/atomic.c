#include <stddef.h>

#include <ps4/error.h>
#include <ps4/atomic.h>

int ps4AtomicSpinLock64(uint64_t *memory)
{
	uint64_t v;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	do
	{
		v = 1;
		ps4AtomicSwap64(memory, &v);
	}
	while(v != 0);

	return PS4_OK;
}

int ps4AtomicSpinUnlock64(uint64_t *memory)
{
	uint64_t v = 0;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4AtomicSwap64(memory, &v);

	if(v != 1)
		return PS4_NOT_OK;
	return PS4_OK;
}

int ps4AtomicSpinLock32(uint32_t *memory)
{
	uint32_t v;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	do
	{
		v = 1;
		ps4AtomicSwap32(memory, &v);
	}
	while(v != 0);

	return PS4_OK;
}

int ps4AtomicSpinUnlock32(uint32_t *memory)
{
	uint32_t v = 0;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4AtomicSwap32(memory, &v);

	if(v != 1)
		return PS4_NOT_OK;
	return PS4_OK;
}

int ps4AtomicSpinLock16(uint16_t *memory)
{
	uint16_t v;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	do
	{
		v = 1;
		ps4AtomicSwap16(memory, &v);
	}
	while(v != 0);

	return PS4_OK;
}

int ps4AtomicSpinUnlock16(uint16_t *memory)
{
	uint16_t v = 0;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4AtomicSwap16(memory, &v);

	if(v != 1)
		return PS4_NOT_OK;
	return PS4_OK;
}

int ps4AtomicSpinLock8(uint8_t *memory)
{
	uint8_t v;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	do
	{
		v = 1;
		ps4AtomicSwap8(memory, &v);
	}
	while(v != 0);

	return PS4_OK;
}

int ps4AtomicSpinUnlock8(uint8_t *memory)
{
	uint8_t v = 0;

	if(memory == NULL)
		return PS4_ERROR_ARGUMENT_PRIMARY_MISSING;

	ps4AtomicSwap8(memory, &v);

	if(v != 1)
		return PS4_NOT_OK;
	return PS4_OK;
}
