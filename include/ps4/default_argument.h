#pragma once

#define Ps4DefaultArgument(argument, type, value) \
	_Generic((argument), type: (value), default: (argument))
