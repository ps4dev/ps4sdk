#pragma once

#include <sys/types.h>
#include <sys/sysent.h>

typedef struct register128_t
{
	union
	{
		struct
		{
			uint64_t low;
			uint64_t high;
		};
		uint64_t value[2];
	};
}
register128_t;

//# movq rax, xmm1 & xmm1 & MOVLHPS
typedef struct Ps4AssemblerRegisterParameter
{
	struct
	{
		union
		{
			struct
			{
				register_t rax;
				register_t rdi;
				register_t rsi;
				register_t rdx;
				register_t rcx;
				register_t r8;
				register_t r9;
			};
			register_t general[7];
		};
		union
		{
			struct
			{
				register128_t xmm0;
				register128_t xmm1;
				register128_t xmm2;
				register128_t xmm3;
				register128_t xmm4;
				register128_t xmm5;
				register128_t xmm6;
				register128_t xmm7;
			};
			register128_t xmm[8];
		};
	};
}
Ps4AssemblerRegisterParameter;

int64_t ps4AssemblerSyscall();

void Ps4AssemblerRegisterParameterPush();
void Ps4AssemblerRegisterParameterPop();
