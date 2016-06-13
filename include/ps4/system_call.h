#pragma once

#include <sys/sysent.h>

typedef struct Ps4SystemCallGenericCallArgument
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
	register_t returns;
}
Ps4SystemCallGenericCallArgument;

typedef struct Ps4SystemCallGenericExecuteArgument
{
	void *function;
	void *uap;
	register_t *returns[2];
}
Ps4SystemCallGenericExecuteArgument;

typedef struct Ps4SystemCallHookArgument Ps4SystemCallGenericHookArgument;

int ps4SystemCallGenericExecute(struct thread *td, Ps4SystemCallGenericExecuteArgument *uap);
int ps4SystemCallGenericCall(struct thread *td, Ps4SystemCallGenericCallArgument *uap);
int ps4SystemCallGenericHook(struct thread *td, Ps4SystemCallGenericHookArgument *uap);
