.pushsection .text

# --- The lie is a condition of life.

.global ps4PayloadTemplateFunctionHookPrologue
.type ps4PayloadTemplateFunctionHookPrologue, @function
ps4PayloadTemplateFunctionHookPrologue:

	# --- fix in args
	movabs $0, %r11

	# --- entry count
	lock incq 16(%r11)

	# --- spin lock
	push %r12
	.Lps4PayloadTemplateFunctionHookPrologueLoop:
		movq $1, %r12
		lock xchg %r12, 184(%r11)
		testq %r12, %r12
		jnz .Lps4PayloadTemplateFunctionHookPrologueLoop
	pop %r12

	# --- No one saves us but ourselves
	sub $184, %rsp
	movdqu %xmm7, 168(%rsp)
	movdqu %xmm6, 152(%rsp)
	movdqu %xmm5, 136(%rsp)
	movdqu %xmm4, 120(%rsp)
	movdqu %xmm3, 104(%rsp)
	movdqu %xmm2, 88(%rsp)
	movdqu %xmm1, 72(%rsp)
	movdqu %xmm0, 56(%rsp)
	movq %r9, 48(%rsp)
	movq %r8, 40(%rsp)
	movq %rcx, 32(%rsp)
	movq %rdx, 24(%rsp)
	movq %rsi, 16(%rsp)
	movq %rdi, 8(%rsp)
	movq %rax, 0(%rsp)

	# --- call clone
	movq %r11, %rdi
	pushq %r11
	call *0(%r11)
	popq %r11

	# --- inc call count
	lock incq 56(%r11)

	# --- r12 to args, fix args to r12
	movq %r12, 32(%rax)
	movq %rax, %r12

	# --- clean stack
	movq 0(%rsp), %rax
	movq 8(%rsp), %rdi
	movq 16(%rsp), %rsi
	movq 24(%rsp), %rdx
	movq 32(%rsp), %rcx
	movq 40(%rsp), %r8
	movq 48(%rsp), %r9
	movdqu 56(%rsp), %xmm0
	movdqu 72(%rsp), %xmm1
	movdqu 88(%rsp), %xmm2
	movdqu 104(%rsp), %xmm3
	movdqu 120(%rsp), %xmm4
	movdqu 136(%rsp), %xmm5
	movdqu 152(%rsp), %xmm6
	movdqu 168(%rsp), %xmm7
	add $184, %rsp

	# --- return to r12
	popq 24(%r12)

	# --- inc call count
	#lock incq 56(%r11)

	 # --- call handler
	jmpq *8(%r11)

.Lps4PayloadTemplateFunctionHookPrologueEnd:
.size ps4PayloadTemplateFunctionHookPrologue, .-ps4PayloadTemplateFunctionHookPrologue

.popsection

.pushsection .rodata

.global ps4PayloadTemplateFunctionHookPrologueSize
.type ps4PayloadTemplateFunctionHookPrologueSize, @object
ps4PayloadTemplateFunctionHookPrologueSize:
.int .Lps4PayloadTemplateFunctionHookPrologueEnd - ps4PayloadTemplateFunctionHookPrologue
.size ps4PayloadTemplateFunctionHookPrologueSize, .-ps4PayloadTemplateFunctionHookPrologueSize

.popsection
