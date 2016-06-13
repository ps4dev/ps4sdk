#pragma once

#include <sys/errno.h>

typedef struct Ps4DefaultExceptionArgument
{
	int code;
}
Ps4DefaultExceptionArgument;

#ifndef Ps4ExceptionTryBegin
	#define Ps4ExceptionTryBegin \
		struct Ps4DefaultExceptionArgument ps4ExceptionValue__ = {0}; \
		goto ps4ExceptionRun__;
#endif

#ifndef Ps4ExceptionTryCatchBegin
	#define Ps4ExceptionTryCatchBegin \
		goto ps4ExceptionCatch__; \
		ps4ExceptionCatch__:
#endif

#ifndef Ps4ExceptionTryCatching
	#define Ps4ExceptionTryCatching(type, name, ...) \
		{ \
			struct type##ExceptionArgument__ name##ExceptionCatchValue__ = {0, __VA_ARGS__}; \
			name##ExceptionCatchValue__.code = ps4ExceptionValue__.code; \
			name##Catch__(name##ExceptionCatchValue__); \
		}
#endif

#ifndef Ps4ExceptionTryFinallyBegin
	#define Ps4ExceptionTryFinallyBegin \
		goto ps4ExceptionFinally__; \
		ps4ExceptionFinally__:
#endif

#ifndef Ps4ExceptionTryFinalizing
	#define Ps4ExceptionTryFinalizing(type, name, ...) \
		{ \
			struct type##ExceptionArgument__ name##ExceptionFinallyValue__ = {0, __VA_ARGS__}; \
			name##ExceptionFinallyValue__.code = ps4ExceptionValue__.code; \
			name##Finally__(name##ExceptionFinallyValue__); \
		}
#endif

#ifndef Ps4ExceptionTryEnd
	#define Ps4ExceptionTryEnd \
		goto ps4ExceptionLeave__; \
		ps4ExceptionLeave__: \
		return ps4ExceptionValue__.code; \
		ps4ExceptionRun__:
#endif

#ifndef Ps4ExceptionTry
	#define Ps4ExceptionTry() \
		Ps4ExceptionTryBegin \
		Ps4ExceptionTryCatchBegin \
		Ps4ExceptionTryFinallyBegin \
		Ps4ExceptionTryEnd
#endif

#ifndef Ps4ExceptionTryCatch
	#define Ps4ExceptionTryCatch(name, ...) \
		Ps4ExceptionTryBegin \
		Ps4ExceptionTryCatchBegin \
		Ps4ExceptionTryCatching(name, name, __VA_ARGS__) \
		Ps4ExceptionTryFinallyBegin \
		Ps4ExceptionTryEnd
#endif

#ifndef Ps4ExceptionTryFinally
	#define Ps4ExceptionTryFinally(name, ...) \
		Ps4ExceptionTryBegin \
		Ps4ExceptionTryCatchBegin \
		Ps4ExceptionTryFinallyBegin \
		Ps4ExceptionTryFinalizing(name, name, __VA_ARGS__) \
		Ps4ExceptionTryEnd
#endif

#ifndef Ps4ExceptionTryCatchFinally
	#define Ps4ExceptionTryCatchFinally(name, ...) \
		Ps4ExceptionTryBegin \
		Ps4ExceptionTryCatchBegin \
		Ps4ExceptionTryCatching(name, name, __VA_ARGS__) \
		Ps4ExceptionTryFinallyBegin \
		Ps4ExceptionTryFinalizing(name, name, __VA_ARGS__) \
		Ps4ExceptionTryEnd
#endif

#ifndef Ps4ExceptionArgument
	#define Ps4ExceptionArgument(name, ...) \
		struct name##ExceptionArgument__ \
		{ \
			int code; \
			__VA_ARGS__; \
		}
#endif

#ifndef Ps4ExceptionCatch
	#define Ps4ExceptionCatch(name) \
		static inline void name##Catch__(struct name##ExceptionArgument__ e)
#endif

#ifndef Ps4ExceptionFinally
	#define Ps4ExceptionFinally(name) \
		static inline void name##Finally__(struct name##ExceptionArgument__ e)
#endif

#ifndef ps4ExceptionThrowIf
	#define ps4ExceptionThrowIf(number, condition) \
		do \
		{ \
			if(condition) \
			{ \
				ps4ExceptionValue__.code = number; \
				goto ps4ExceptionCatch__; \
			} \
		} \
		while(0)
#endif

#ifndef ps4ExceptionAssert
	#define ps4ExceptionAssert(number, condition) \
		ps4ExceptionThrowIf(number, condition)
#endif

#ifndef ps4ExceptionThrow
	#define ps4ExceptionThrow(number) \
		ps4ExceptionThrowIf(number, 1)
#endif

#ifndef ps4ExceptionRethrow
	#define ps4ExceptionRethrow(error) \
		do \
		{ \
			if(error) \
			{ \
				ps4ExceptionValue__.code = error; \
				goto ps4ExceptionCatch__; \
			} \
		} \
		while(0)
#endif

#ifndef ps4ExceptionReturnIf
	#define ps4ExceptionReturnIf(number, condition) \
		do \
		{ \
			if(condition) \
			{ \
				ps4ExceptionValue__.code = number; \
				goto ps4ExceptionFinally__; \
			} \
		} \
		while(0)
#endif

#ifndef ps4ExceptionReturn
	#define ps4ExceptionReturn(number) \
		 ps4ExceptionReturnIf(number, 1)
#endif

#ifndef ps4ExceptionFail
	#define ps4ExceptionFail(number) \
		 ps4ExceptionReturnIf(number, 1)
#endif

#ifndef ps4ExceptionFailIf
	#define ps4ExceptionFailIf(number, condition) \
		 ps4ExceptionReturnIf(number, condition)
#endif

#ifndef ps4ExceptionSuccess
	#define ps4ExceptionSuccess() \
		 ps4ExceptionReturnIf(0, 1)
#endif

#ifndef ps4ExceptionSuccessIf
	#define ps4ExceptionSuccessIf(condition) \
		 ps4ExceptionReturnIf(0, condition)
#endif

#ifndef ps4ExceptionLeaveIf
	#define ps4ExceptionLeaveIf(number, condition) \
		do \
		{ \
			if(condition) \
			{ \
				ps4ExceptionValue__.code = number; \
				goto ps4ExceptionLeave__; \
			} \
		} \
		while(0)
#endif

#ifndef ps4ExceptionLeave
	#define ps4ExceptionLeave(number) \
		 ps4ExceptionLeaveIf(number, 1)
#endif
