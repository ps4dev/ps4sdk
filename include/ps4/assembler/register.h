#pragma once

typedef struct register128_t
{
	union
	{
		struct
		{
			register_t low;
			register_t high;
		};
		register_t value[2];
	};
}
register128_t;

typedef struct Ps4RegisterArguments
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
Ps4RegisterArguments;

void ps4AssemblerRegisterPushAllArguments();
void ps4AssemblerRegisterPopAllArguments();

void ps4AssemblerRegisterPushAllArgumentsToR11();
void ps4AssemblerRegisterPopAllArgumentsFromR11();
