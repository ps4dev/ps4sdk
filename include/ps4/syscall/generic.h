#pragma once

typedef struct Ps4SyscallGenericCallArgument
{
	void *function;
	union
	{
		struct
		{
			register_t rdi;
			register_t rsi;
			register_t rdx;
			register_t rcx;
			register_t r8;
			register_t r9;
		};
		register_t argument[6];
	};
	register_t returned;
}
Ps4SyscallGenericCallArgument;

typedef struct Ps4SyscallGenericExecuteArgument
{
	void *function;
	void *uap;
	register_t *returned[2];
}
Ps4SyscallGenericExecuteArgument;

int ps4SyscallGenericExecute(struct thread *td, Ps4SyscallGenericExecuteArgument *uap);
int ps4SyscallGenericCall(struct thread *td, Ps4SyscallGenericCallArgument *uap);
