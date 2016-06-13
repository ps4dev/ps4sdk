# This syscall can be used in a normal (clean) process
# SystemCall stubs and the resolvers are based on the symbol
# You would normally not use non-libc-wrapped syscalls in a clean process
# but you must supply the symbol for the adaptive resolvers

.pushsection .text
.global ps4AssemblerSystemCall
.type ps4AssemblerSystemCall, @function
ps4AssemblerSystemCall:
	movq %rcx, %r10
	syscall
	jnc .Lps4AssemblerSystemCallR
	movq $-1, %rax
	.Lps4AssemblerSystemCallR:
		ret
.size ps4AssemblerSystemCall, .-ps4AssemblerSystemCall
.popsection
