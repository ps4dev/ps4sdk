#pragma once

#define Ps4ArgumentFirst(x, ...) x
#define Ps4ArgumentRest(x, ...) __VA_ARGS__

#ifndef ps4ExpressionDoIf
	#define ps4ExpressionDoIf(expr, condition) \
		if(condition) { expr; }
#endif

#ifndef ps4ExpressionReturnOnError
	#define ps4ExpressionReturnOnError(val) \
		if(val) { return val; }
#endif

#ifndef ps4ExpressionReturnIf
	#define ps4ExpressionReturnIf(val, condition) \
		if(condition) { return val; }
#endif

#ifndef ps4FunctionExecuteIf
	#define ps4FunctionExecuteIf(expr, condition) \
		if(condition) { expr(); }
#endif

#ifndef ps4FunctionCallIf
	#define ps4FunctionCallIf(fn, condition, ...) \
		if(condition) { fn(__VA_ARGS__); }
#endif

#ifndef ps4FunctionCallIfNotNull
	#define ps4FunctionCallIfNotNull(fn, arg) \
		if(arg) { fn(arg); }
#endif

#ifndef ps4FunctionCallIfPrimaryNotNull
	#define ps4FunctionCallIfPrimaryNotNull(fn, ...) \
		if(Ps4ArgumentFirst(__VA_ARGS__)) { fn(__VA_ARGS__); }
#endif
