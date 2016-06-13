.pushsection .text

.global ps4PayloadFunctionHookHandler
.type ps4PayloadFunctionHookHandler, @function
ps4PayloadFunctionHookHandler:

	# --- entry count - FIXME: should be in handler -> races free
	pushq %r12
	lock decq 16(%r11) # --- decrease entry count
	movq $0, %r12
	lock xchg %r12, 184(%r11) # --- unlock
	popq %r12

	# --- regs to args
	movq 64(%r12), %r11 #arguments
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

	# --- hook type, pre and post loop
	movq $0, 144(%r12) # --- hook type
	.Lps4PayloadFunctionHookHandlerPostRun:

	# --- loop thru hooks, call
	movq $-1, 120(%r12)
	.Lps4PayloadFunctionHookHandlerLoop:
		incq 120(%r12)

		# -- hook pre post run
		movq 144(%r12), %rdi # --- hook type
		movq 112(%r12), %r11
		movq 120(%r12), %r8
		movq (%r11, %r8, 8), %r11

		# -- all hooks done - null terminator
		testq %r11, %r11
		jz .Lps4PayloadFunctionHookHandlerLoopDone

		cmpq $0, %rdi
		jne .Lps4PayloadFunctionHookHandlerLoopPostCheck

		cmpq $0, %r11
		je .Lps4PayloadFunctionHookHandlerLoop
		cmpq $2, %r11
		je .Lps4PayloadFunctionHookHandlerLoop
		jmp .Lps4PayloadFunctionHookHandlerLoopPostCheckEnd

		.Lps4PayloadFunctionHookHandlerLoopPostCheck:
		cmpq $0, %r11
		je .Lps4PayloadFunctionHookHandlerLoop
		cmpq $-1, %r11
		je .Lps4PayloadFunctionHookHandlerLoop
		cmpq $1, %r11
		je .Lps4PayloadFunctionHookHandlerLoop

		.Lps4PayloadFunctionHookHandlerLoopPostCheckEnd:
		# -- hook type
		cmpq $0, %r11
		jg .Lps4PayloadFunctionHookHandlerLoopGeneric
		jl .Lps4PayloadFunctionHookHandlerLoopSpecific
		jmp .Lps4PayloadFunctionHookHandlerLoop

		.Lps4PayloadFunctionHookHandlerLoopSpecific:
		mov %gs:0x0,%rdi
		movq %r12, 0x388(%rdi)
		movq 64(%r12), %r11 #arguments
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
		jmp .Lps4PayloadFunctionHookHandlerLoopCall

		.Lps4PayloadFunctionHookHandlerLoopGeneric:
		mov %gs:0x0,%rdi
		#movq %r12, 0x388(%rdi)
		movq %r12, %rsi

		.Lps4PayloadFunctionHookHandlerLoopCall:
		movq 104(%r12), %r11
		movq 120(%r12), %r8
		movq (%r11, %r8, 8), %r11
		# -- all hooks done - null terminator
		testq %r11, %r11
		jz .Lps4PayloadFunctionHookHandlerLoopDone

		call *%r11

		# -- hook return != 0, cancel
		cmpq $0, %rax
		je .Lps4PayloadFunctionHookHandlerLoop
		jg .Lps4PayloadFunctionHookHandlerLoopDone # --- r > 0, force call
		jmp .Lps4PayloadFunctionHookHandlerCallDone
		#movq 64(%r12), %r11 # --- arguments
		#movq 0(%r11), %rax
		#movq 24(%r11), %rdx
		#movdqu 56(%r11), %xmm0
		#movdqu 72(%r11), %xmm1
		#jmp .Lps4PayloadFunctionHookHandlerSkipCall # --- r < 0, skip call

	.Lps4PayloadFunctionHookHandlerLoopDone:

	# --- args back to regs
	movq 64(%r12), %r11 #arguments
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

	# --- check if this is a pre or post run
	movq $1, %r11
	xchgq %r11, 144(%r12) # --- hook type
	testq %r11, %r11
	jnz .Lps4PayloadFunctionHookHandlerCallDone

	# --- When you want to fool the world, tell the truth.
	callq *88(%r12)

	# --- returnes to args
	movq 72(%r12), %r11
	movq %rax, 0(%r11) # --- returned[0]
	movq %rdx, 8(%r11) # --- returned[1]
	movdqu %xmm0, 16(%r11)
	movdqu %xmm1, 32(%r11)

	jmp .Lps4PayloadFunctionHookHandlerPostRun
	.Lps4PayloadFunctionHookHandlerCallDone:

	#.Lps4PayloadFunctionHookHandlerSkipCall:

	# --- returnes from args
	movq 72(%r12), %r11
	movq 0(%r11), %rax # --- returned[0]
	movq 8(%r11), %rdx # --- returned[1]
	movdqu 16(%r11), %xmm0
	movdqu 32(%r11), %xmm1

	# --- move stuff back from arguments
	pushq 24(%r12) # --- return
	movq 32(%r12), %r11 # --- r12 saved to argument
	xchg %r11, %r12 # -- reset r12 keep args as r11

	# --- rets safed for call
	sub $48, %rsp
	movdqu %xmm1, 32(%rsp)
	movdqu %xmm0, 16(%rsp)
	movq %rdx, 8(%rsp)
	movq %rax, 0(%rsp)

	# --- call clean / epilogue
	movq %r11, %rdi
	call *48(%r11)

	# --- rets back
	movq 0(%rsp), %rax
	movq 8(%rsp), %rdx
	movdqu 16(%rsp), %xmm0
	movdqu 32(%rsp), %xmm1
	add $48, %rsp

	# --- profit ... *nyan*
	retq

.Lps4PayloadFunctionHookHandlerEnd:
.size ps4PayloadFunctionHookHandler, .-ps4PayloadFunctionHookHandler

.popsection

.pushsection .rodata

.global ps4PayloadFunctionHookHandlerSize
.type ps4PayloadFunctionHookHandlerSize, @object
ps4PayloadFunctionHookHandlerSize:
.int .Lps4PayloadFunctionHookHandlerEnd - ps4PayloadFunctionHookHandler
.size ps4PayloadFunctionHookHandlerSize, .-ps4PayloadFunctionHookHandlerSize

.popsection
