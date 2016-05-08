# This syscall can be used in a normal (clean) process
# Syscall stubs and the resolvers are based on the symbol
# You would normally not use non-libc-wrapped syscalls in a clean process
# but you must supply the symbol for the adaptive resolvers

.pushsection .text
.global ps4AssemblerSyscall
.type ps4AssemblerSyscall, @function
ps4AssemblerSyscall:
	syscall
	jnc .Lps4AssemblerSyscallR
	movq $-1, %rax
	.Lps4AssemblerSyscallR:
		ret
.size ps4AssemblerSyscall, .-ps4AssemblerSyscall
.popsection
