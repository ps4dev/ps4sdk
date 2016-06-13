#pragma once

#include <sys/syscall.h>
#include <ps4/stub_resolve.h>
#include <ps4/assembler.h>

#ifndef Ps4StubModuleSymbol
	#define Ps4StubModuleSymbol(name) \
		name
#endif
#ifndef Ps4StubModuleNameSymbol
	#define Ps4StubModuleNameSymbol(name) \
		name##Name
#endif

#ifndef Ps4StubFunctionSymbol
	#define Ps4StubFunctionSymbol(name) \
		name
#endif
#ifndef Ps4StubFunctionNameSymbol
	#define Ps4StubFunctionNameSymbol(name) \
		.L##name##Name
#endif
#ifndef Ps4StubFunctionAddressSymbol
	#define Ps4StubFunctionAddressSymbol(name) \
		.L##name##Address
#endif
#ifndef Ps4StubKernFunctionAddressSymbol
	#define Ps4StubKernFunctionAddressSymbol(name) \
		.L##name##KernelAddress
#endif

#ifndef Ps4StubSystemCallSymbol
	#define Ps4StubSystemCallSymbol(name) name
#endif
#ifndef Ps4StubSystemCallNumber
	#define Ps4StubSystemCallNumber(name) SYS_##name
#endif

#ifndef Ps4StubData
	#define Ps4StubData(section, scope, name, type, value) \
		Ps4StubData_(section, scope, name, type, value)

	#define Ps4StubData_(section, scope, name, type, value) \
		__asm__(" \
			.pushsection "#section" \n \
			"#scope" "#name" \n \
			.type "#name", "#type" \n \
			"#name": \n \
			"#value" \n \
			.size "#name", .-"#name" \n \
			.popsection \n \
		");
#endif

#ifndef Ps4StubModuleData
	#define Ps4StubModuleHandleData(name) \
		Ps4StubModuleHandleData_(name)
	#define Ps4StubModuleHandleData_(name) \
		Ps4StubData(.bss, .global, name, @object, .zero 8)
#endif

#ifndef Ps4StubModuleNameData
	#define Ps4StubModuleNameData(name, value) \
		Ps4StubModuleNameData_(name, value)
	#define Ps4StubModuleNameData_(name, value) \
		Ps4StubData(.rodata, .global, name, @object, .asciz #value)
#endif

#ifndef Ps4StubFunctionAddressData
	#define Ps4StubFunctionAddressData(name) \
		Ps4StubFunctionAddressData_(name)
	#define Ps4StubFunctionAddressData_(name) \
		Ps4StubData(.bss, .local, name, @object, .zero 8)
#endif

#ifndef Ps4StubKernFunctionAddressData
	#define Ps4StubKernFunctionAddressData(name) \
		Ps4StubKernFunctionAddressData_(name)
	#define Ps4StubKernFunctionAddressData_(name) \
		Ps4StubData(.bss, .local, name, @object, .zero 8)
#endif

#ifndef Ps4StubFunctionNameData
	#define Ps4StubFunctionNameData(name, value) \
		Ps4StubFunctionNameData_(name, value)
	#define Ps4StubFunctionNameData_(name, value) \
		Ps4StubData(.rodata, .local, name, @object, .asciz #value)
#endif

#ifndef Ps4StubModule
	#define Ps4StubModule(name) \
		Ps4StubModule_(name)
	#define Ps4StubModule_(name) \
 		Ps4StubModuleHandleData(Ps4StubModuleSymbol(name)) \
		Ps4StubModuleNameData(Ps4StubModuleNameSymbol(name), name.sprx)
#endif

// if you call these in kernel mode you shoot yourself (sometimes) (could check)
#ifndef Ps4StubFunctionStub
	#define Ps4StubFunctionStub(function, moduleName, functionName, moduleHandle, functionAddress) \
		Ps4StubFunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress)

	#define Ps4StubFunctionStub_(function, moduleName, functionName, moduleHandle, functionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $0, %al #this is a two byte type marker \n \
				mov %rax, %r11 \n \
				movabs "#functionAddress", %rax \n \
				xchg %rax, %r11 \n \
				test %r11, %r11 \n \
				jz .L"#function"R \n \
					jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4AssemblerRegisterParameterPush \n \
					movabs $"#function", %rdi \n \
					movabs $"#moduleName", %rsi \n \
					movabs $"#functionName", %rdx \n \
					movabs $"#moduleHandle", %rcx \n \
					movabs $"#functionAddress", %r8 \n \
					xor %rax, %rax \n \
					call ps4StubResolve \n \
					mov %rax, %r11 \n \
					call ps4AssemblerRegisterParameterPop \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

// if you call these in user mode you shoot yourself (could check)
#ifndef Ps4StubKernFunctionStub
	#define Ps4StubKernFunctionStub(function, functionName, functionAddress) \
		Ps4StubKernFunctionStub_(function, functionName, functionAddress)

	#define Ps4StubKernFunctionStub_(function, functionName, functionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $1, %al \n \
				mov %rax, %r11 \n \
				movabs "#functionAddress", %rax \n \
				xchg %rax, %r11 \n \
				test %r11, %r11 \n \
				jz .L"#function"R \n \
					jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4AssemblerRegisterParameterPush \n \
					movabs $"#function", %rdi \n \
					movabs $"#functionName", %rsi \n \
					movabs $"#functionAddress", %rdx \n \
					xor %rax, %rax \n \
					call ps4KernelStubResolve \n \
					mov %rax, %r11 \n \
					call ps4AssemblerRegisterParameterPop \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

// use rsp to check ring 0 ...
// if so, resolve and user kernel fn
// otherwise use useland stuff
#ifndef Ps4StubAdaptiveFunctionStub
	#define Ps4StubAdaptiveFunctionStub(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress) \
		Ps4StubAdaptiveFunctionStub_(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress)

	#define Ps4StubAdaptiveFunctionStub_(function, moduleName, functionName, moduleHandle, userFunctionAddress, kernelFunctionAddress) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $2, %al \n \
				mov %rax, %r11 \n \
				movq %rsp, %rax \n \
				shrq $48, %rax \n \
				test %rax, %rax \n \
				jnz .L"#function"K \n \
					movabs "#userFunctionAddress", %rax \n \
					jmp .L"#function"C \n \
				.L"#function"K: \n \
					movabs "#kernelFunctionAddress", %rax \n \
				.L"#function"C: \n \
					xchg %rax, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"R \n \
						jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4AssemblerRegisterParameterPush \n \
					movabs $"#function", %rdi \n \
					movabs $"#moduleName", %rsi \n \
					movabs $"#functionName", %rdx \n \
					movabs $"#moduleHandle", %rcx \n \
					movabs $"#userFunctionAddress", %r8 \n \
					movabs $"#kernelFunctionAddress", %r9 \n \
					xor %rax, %rax \n \
					call ps4AdaptiveStubResolve \n \
					mov %rax, %r11 \n \
					call ps4AssemblerRegisterParameterPop \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef Ps4StubAdaptiveSystemCallStub
	#define Ps4StubAdaptiveSystemCallStub(function, functionName, functionAddress, number) \
		Ps4StubAdaptiveSystemCallStub_(function, functionName, functionAddress, number)

	#define Ps4StubAdaptiveSystemCallStub_(function, functionName, functionAddress, number) \
		__asm__(" \
			.pushsection .text \n \
			.global "#function" \n \
			.type "#function", @function \n \
			"#function": \n \
				test $3, %al \n \
				mov %rax, %r11 \n \
				movq %rsp, %rax \n \
				shrq $48, %rax \n \
				test %rax, %rax \n \
				jnz .L"#function"K \n \
					movq $"#number", %rax \n \
					jmp ps4AssemblerSystemCall \n \
				.L"#function"K: \n \
					movabs "#functionAddress", %rax \n \
					xchg %rax, %r11 \n \
					test %r11, %r11 \n \
					jz .L"#function"R \n \
						jmp *%r11 \n \
				.L"#function"R: \n \
					call ps4AssemblerRegisterParameterPush \n \
					movabs $"#function", %rdi \n \
					movabs $"#functionName", %rsi \n \
					movabs $"#functionAddress", %rdx \n \
					xor %rax, %rax \n \
					call ps4KernelStubResolve \n \
					mov %rax, %r11 \n \
					call ps4AssemblerRegisterParameterPop \n \
					test %r11, %r11 \n \
					jnz .L"#function"E \n \
					jmp "#function" \n \
				.L"#function"E: \n \
					ret \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef Ps4StubSystemCallStub
	#define Ps4StubSystemCallStub(function, number) \
		Ps4StubSystemCallStub_(function, number)

	#define Ps4StubSystemCallStub_(function, number) \
		__asm__("\
			.pushsection .text \n \
			.global "#function"\n \
			.type "#function", @function \n \
		 	"#function":\n \
				test $4, %al \n \
				movq $"#number", %rax \n \
				jmp ps4AssemblerSystemCall \n \
			.size "#function", .-"#function" \n \
			.popsection \n \
		");
#endif

#ifndef Ps4StubSystemCallNamed
	#define Ps4StubSystemCallNamed(name, number) \
		Ps4StubSystemCallNamed_(name, number)

	#define Ps4StubSystemCallNamed_(name, number) \
		Ps4StubSystemCallStub(Ps4StubSystemCallSymbol(name), number)
#endif

#ifndef SystemCall
	#define Ps4StubSystemCall(name) \
		Ps4StubSystemCall_(name)

	#define Ps4StubSystemCall_(name) \
		Ps4StubSystemCallStub(Ps4StubSystemCallSymbol(name), Ps4StubSystemCallNumber(name))
#endif

#ifndef Ps4StubFunction
	#define Ps4StubFunction(module, name) \
		Ps4StubFunction_(module, name)

	#define Ps4StubFunction_(module, name) \
		Ps4StubFunctionNameData(Ps4StubFunctionNameSymbol(name), Ps4StubFunctionSymbol(name)) \
		Ps4StubFunctionAddressData(Ps4StubFunctionAddressSymbol(name)) \
		Ps4StubFunctionStub(Ps4StubFunctionSymbol(name), Ps4StubModuleNameSymbol(module), Ps4StubFunctionNameSymbol(name),  Ps4StubModuleSymbol(module), Ps4StubFunctionAddressSymbol(name))
#endif

#ifndef Ps4StubKernFunction
	#define Ps4StubKernFunction(name) \
		Ps4StubKernFunction_(name)

	#define Ps4StubKernFunction_(name) \
		Ps4StubFunctionNameData(Ps4StubFunctionNameSymbol(name), Ps4StubFunctionSymbol(name)) \
		Ps4StubKernFunctionAddressData(Ps4StubKernFunctionAddressSymbol(name)) \
		Ps4StubKernFunctionStub(Ps4StubFunctionSymbol(name), Ps4StubFunctionNameSymbol(name), Ps4StubKernFunctionAddressSymbol(name))
#endif

#ifndef Ps4StubAdaptiveFunction
	#define Ps4StubAdaptiveFunction(module, name) \
		Ps4StubAdaptiveFunction_(module, name)

	#define Ps4StubAdaptiveFunction_(module, name) \
		Ps4StubFunctionNameData(Ps4StubFunctionNameSymbol(name), Ps4StubFunctionSymbol(name)) \
		Ps4StubFunctionAddressData(Ps4StubFunctionAddressSymbol(name)) \
		Ps4StubKernFunctionAddressData(Ps4StubKernFunctionAddressSymbol(name)) \
		Ps4StubAdaptiveFunctionStub(Ps4StubFunctionSymbol(name), Ps4StubModuleNameSymbol(module), Ps4StubFunctionNameSymbol(name),  Ps4StubModuleSymbol(module), Ps4StubFunctionAddressSymbol(name), Ps4StubKernFunctionAddressSymbol(name))
#endif

#ifndef Ps4StubAdaptiveSystemCall
	#define Ps4StubAdaptiveSystemCall(name) \
		Ps4StubAdaptiveSystemCall_(name)

	#define Ps4StubAdaptiveSystemCall_(name) \
		Ps4StubFunctionNameData(Ps4StubFunctionNameSymbol(name), Ps4StubFunctionSymbol(name)) \
		Ps4StubKernFunctionAddressData(Ps4StubKernFunctionAddressSymbol(name)) \
		Ps4StubAdaptiveSystemCallStub(Ps4StubFunctionSymbol(name), Ps4StubFunctionNameSymbol(name), Ps4StubKernFunctionAddressSymbol(name), Ps4StubSystemCallNumber(name))
#endif
