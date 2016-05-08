#pragma once

#define Ps4ArgumentFirst(x, ...) x
#define Ps4ArgumentRest(x, ...) __VA_ARGS__

#ifndef ps4ExpressionDoIf
	#define ps4ExpressionDoIf(expr, condition) \
		do \
		{ \
			if(condition) { expr; } \
		} \
		while(0)
#endif

#ifndef ps4FunctionExecuteIf
	#define ps4FunctionExecuteIf(expr, condition) \
		do \
		{ \
			if(condition) { expr(); } \
		} \
		while(0)
#endif

#ifndef ps4FunctionCallIf
	#define ps4FunctionCallIf(fn, condition, ...) \
		do \
		{ \
			if(condition) { fn(__VA_ARGS__); } \
		} \
		while(0)
#endif

#ifndef ps4FunctionCallIfNotNull
	#define ps4FunctionCallIfNotNull(fn, arg) \
		do \
		{ \
			if(arg) { fn(arg); } \
		} \
		while(0)
#endif

#ifndef ps4FunctionCallIfPrimaryNotNull
	#define ps4FunctionCallIfPrimaryNotNull(fn, ...) \
		do \
		{ \
			if(Ps4ArgumentFirst(__VA_ARGS__)) { fn(__VA_ARGS__); } \
		} \
		while(0)
#endif
