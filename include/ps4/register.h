#pragma once

/*
typedef struct
{
	unsigned int gp_offset;
	unsigned int fp_offset;
	void *overflow_arg_area;
	void *reg_save_area;
}
va_list[1];
*/

typedef struct Ps4ArgumentVariableList
{
	unsigned int generalPurposeRegisterOffset;
	unsigned int floatPointRegisterOffset;
	void *overflowArgumentArea;
	void *registerSaveArea;
}
Ps4ArgumentVariableList;

typedef struct Ps4UnsignedInteger128
{
	uint64_t low;
	uint64_t high;
}
Ps4UnsignedInteger128;

typedef struct Ps4RegisterGeneralPurposes
{
	int64_t rdi;
	int64_t rsi;
	int64_t rdx;
	int64_t rcx;
	int64_t r8;
	int64_t r9;
}
Ps4RegisterGeneralPurposes;

typedef struct Ps4RegisterFloatPointsSmall
{
	Ps4UnsignedInteger128 xmm0;
	Ps4UnsignedInteger128 xmm1;
	Ps4UnsignedInteger128 xmm2;
	Ps4UnsignedInteger128 xmm3;
	Ps4UnsignedInteger128 xmm4;
	Ps4UnsignedInteger128 xmm5;
	Ps4UnsignedInteger128 xmm6;
	Ps4UnsignedInteger128 xmm7;
}
Ps4RegisterFloatPointsSmall;


typedef struct Ps4RegisterFloatPoints
{
	Ps4UnsignedInteger128 xmm0;
	Ps4UnsignedInteger128 xmm1;
	Ps4UnsignedInteger128 xmm2;
	Ps4UnsignedInteger128 xmm3;
	Ps4UnsignedInteger128 xmm4;
	Ps4UnsignedInteger128 xmm5;
	Ps4UnsignedInteger128 xmm6;
	Ps4UnsignedInteger128 xmm7;
	Ps4UnsignedInteger128 xmm8;
	Ps4UnsignedInteger128 xmm9;
	Ps4UnsignedInteger128 xmm10;
	Ps4UnsignedInteger128 xmm11;
	Ps4UnsignedInteger128 xmm12;
	Ps4UnsignedInteger128 xmm13;
	Ps4UnsignedInteger128 xmm14;
	Ps4UnsignedInteger128 xmm15;
}
Ps4RegisterFloatPoints;

typedef struct Ps4RegisterSaveArea
{
	int64_t rdi;
	int64_t rsi;
	int64_t rdx;
	int64_t rcx;
	int64_t r8;
	int64_t r9;
	Ps4UnsignedInteger128 xmm0;
	Ps4UnsignedInteger128 xmm1;
	Ps4UnsignedInteger128 xmm2;
	Ps4UnsignedInteger128 xmm3;
	Ps4UnsignedInteger128 xmm4;
	Ps4UnsignedInteger128 xmm5;
	Ps4UnsignedInteger128 xmm6;
	Ps4UnsignedInteger128 xmm7;
	Ps4UnsignedInteger128 xmm8;
	Ps4UnsignedInteger128 xmm9;
	Ps4UnsignedInteger128 xmm10;
	Ps4UnsignedInteger128 xmm11;
	Ps4UnsignedInteger128 xmm12;
	Ps4UnsignedInteger128 xmm13;
	Ps4UnsignedInteger128 xmm14;
	Ps4UnsignedInteger128 xmm15;
}
Ps4RegisterSaveArea;

typedef struct Ps4RegisterParameters
{
	int64_t rax;
	int64_t rdi;
	int64_t rsi;
	int64_t rdx;
	int64_t rcx;
	int64_t r8;
	int64_t r9;
	Ps4UnsignedInteger128 xmm0;
	Ps4UnsignedInteger128 xmm1;
	Ps4UnsignedInteger128 xmm2;
	Ps4UnsignedInteger128 xmm3;
	Ps4UnsignedInteger128 xmm4;
	Ps4UnsignedInteger128 xmm5;
	Ps4UnsignedInteger128 xmm6;
	Ps4UnsignedInteger128 xmm7;
}
Ps4RegisterParameters;

typedef struct Ps4RegisterReturns
{
	int64_t rax;
	int64_t rdx;
	Ps4UnsignedInteger128 xmm0;
	Ps4UnsignedInteger128 xmm1;
}
Ps4RegisterReturns;
