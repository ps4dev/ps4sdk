# This rop syscall is used in a exploited jit webbrowser process
# The rop address is stored in a fixed location and loaded to r11

.pushsection .text
.global ps4AssemblerSyscall
.type ps4AssemblerSyscall, @function
ps4AssemblerSyscall:
	movabs $0x93a4FFFF8, %r11
	movq (%r11), %r11
	movq %rcx, %r10
	call *%r11
	jnc .Lps4SyscallR
	# The error and return value is rax + carry (freebsd, systemv abi)
	# Be aware that this non-libc-syscall does not set libc's errno
	# as we may not even use libc at this point, or at all
	movq $-1, %rax
	.Lps4SyscallR:
		ret
.size ps4AssemblerSyscall, .-ps4AssemblerSyscall
.popsection
