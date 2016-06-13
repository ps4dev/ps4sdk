# This rop syscall is used in a exploited jit webbrowser process
# The rop address is stored in a fixed location and loaded to r11

.pushsection .text
.global ps4AssemblerSystemCall
.type ps4AssemblerSystemCall, @function
ps4AssemblerSystemCall:
	movabs $0x93a4FFFF8, %r11
	movq (%r11), %r11
	movq %rcx, %r10
	call *%r11
	jnc .Lps4AssemblerSystemCallR
	# The error and return value is rax + carry (freebsd, systemv abi)
	# Be aware that this non-libc-syscall does not set libc's errno
	# as we may not even use libc at this point, or at all
	movq $-1, %rax
	.Lps4AssemblerSystemCallR:
		ret
.size ps4AssemblerSystemCall, .-ps4AssemblerSystemCall
.popsection
