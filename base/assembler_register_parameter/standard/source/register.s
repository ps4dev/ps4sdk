.pushsection .text

.global Ps4AssemblerRegisterParameterPush
.type Ps4AssemblerRegisterParameterPush, @function
Ps4AssemblerRegisterParameterPush:
	sub $192, %rsp
	movq %rax, 8(%rsp)
	movq 192(%rsp), %rax
	movdqu %xmm7, 176(%rsp)
	movdqu %xmm6, 160(%rsp)
	movdqu %xmm5, 144(%rsp)
	movdqu %xmm4, 128(%rsp)
	movdqu %xmm3, 112(%rsp)
	movdqu %xmm2, 96(%rsp)
	movdqu %xmm1, 80(%rsp)
	movdqu %xmm0, 64(%rsp)
	movq %r9, 56(%rsp)
	movq %r8, 48(%rsp)
	movq %rcx, 40(%rsp)
	movq %rdx, 32(%rsp)
	movq %rsi, 24(%rsp)
	movq %rdi, 16(%rsp)
	movq %rax, 0(%rsp)
	ret
.size Ps4AssemblerRegisterParameterPush, .-Ps4AssemblerRegisterParameterPush

.global Ps4AssemblerRegisterParameterPop
.type Ps4AssemblerRegisterParameterPop, @function
Ps4AssemblerRegisterParameterPop:
	movq 0(%rsp), %rax
	movq 16(%rsp), %rdi
	movq 24(%rsp), %rsi
	movq 32(%rsp), %rdx
	movq 40(%rsp), %rcx
	movq 48(%rsp), %r8
	movq 56(%rsp), %r9
	movdqu 64(%rsp), %xmm0
	movdqu 80(%rsp), %xmm1
	movdqu 96(%rsp), %xmm2
	movdqu 112(%rsp), %xmm3
	movdqu 128(%rsp), %xmm4
	movdqu 144(%rsp), %xmm5
	movdqu 160(%rsp), %xmm6
	movdqu 176(%rsp), %xmm7
	movq %rax, 192(%rsp)
	movq 8(%rsp), %rax
	add $192, %rsp
	ret
.size Ps4AssemblerRegisterParameterPop, .-Ps4AssemblerRegisterParameterPop

.popsection
