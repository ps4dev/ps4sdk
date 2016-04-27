.pushsection .text

.global ps4AssemblerRegisterPushAllArgumentsToR11
.type ps4AssemblerRegisterPushAllArgumentsToR11, @function
ps4AssemblerRegisterPushAllArgumentsToR11:
	movdqu %xmm7, 168(%r11)
	movdqu %xmm6, 152(%r11)
	movdqu %xmm5, 136(%r11)
	movdqu %xmm4, 120(%r11)
	movdqu %xmm3, 104(%r11)
	movdqu %xmm2, 88(%r11)
	movdqu %xmm1, 72(%r11)
	movdqu %xmm0, 56(%r11)
	movq %r9, 48(%r11)
	movq %r8, 40(%r11)
	movq %rcx, 32(%r11)
	movq %rdx, 24(%r11)
	movq %rsi, 16(%r11)
	movq %rdi, 8(%r11)
	movq %rax, 0(%r11)
	ret
.size ps4AssemblerRegisterPushAllArgumentsToR11, .-ps4AssemblerRegisterPushAllArgumentsToR11

.global ps4AssemblerRegisterPopAllArgumentsFromR11
.type ps4AssemblerRegisterPopAllArgumentsFromR11, @function
ps4AssemblerRegisterPopAllArgumentsFromR11:
	movq 0(%r11), %rax
	movq 8(%r11), %rdi
	movq 16(%r11), %rsi
	movq 24(%r11), %rdx
	movq 32(%r11), %rcx
	movq 40(%r11), %r8
	movq 48(%r11), %r9
	movdqu 56(%r11), %xmm0
	movdqu 72(%r11), %xmm1
	movdqu 88(%r11), %xmm2
	movdqu 104(%r11), %xmm3
	movdqu 120(%r11), %xmm4
	movdqu 136(%r11), %xmm5
	movdqu 152(%r11), %xmm6
	movdqu 168(%r11), %xmm7
	ret
.size ps4AssemblerRegisterPopAllArgumentsFromR11, .-ps4AssemblerRegisterPopAllArgumentsFromR11

.popsection
