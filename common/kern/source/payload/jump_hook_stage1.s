.pushsection .text

.global ps4PayloadJumpHookStage1
.type ps4PayloadJumpHookStage1, @function
ps4PayloadJumpHookStage1:

	# --- safe and replace %r12
	movq %r12, 104(%r11) # --- r12 saved to argument
	movq %r11, %r12 # --- argument fixed to r12

	# --- No one saves us but ourselves
	mov 56(%r12), %r11 # --- arguments
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

	# -- pop return to arg, clean stack
	popq 96(%r12)

	# -- count calls, for fun ... and profit ...
	lock incq 88(%r12)

	# --- loop thru hooks, call
	movq $0, 16(%r12)
	.Lps4PayloadJumpHookStage1Loop:
		movq %gs:0, %rdi
		movq %r12, %rsi
		movq 8(%r12), %r11
		movq 16(%r12), %r8
		movq (%r11, %r8, 8), %r11
		# -- all hooks done - null terminator
		testq %r11, %r11
		jz .Lps4PayloadJumpHookStage1LoopDone
		call *%r11
		# --- current ++
		incq 16(%r12)
		# -- hook return != 0, cancel
		cmpq $0, %rax
		jg .Lps4PayloadJumpHookStage1LoopDone
		je .Lps4PayloadJumpHookStage1Loop
		movq 56(%r12), %r11 # --- arguments
		movq 0(%r11), %rax
		jmp .Lps4PayloadJumpHookStage1SkipCall
	.Lps4PayloadJumpHookStage1LoopDone:

	# --- args from 48 to regs
	movq 56(%r12), %r11 #arguments
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

	# --- move stuff back from arguments
	pushq 96(%r12)
	movq 104(%r12), %r11 # --- r12 saved to argument
	xchg %r11, %r12 # -- reset r12 keep args as r11

	push %r12
	lock decq 80(%r11) # --- decrease entry count
	movq $0, %r12
	lock xchg %r12, 64(%r11) # --- unlock
	pop %r12

	# --- When you want to fool the world, tell the truth.
	jmpq *40(%r11)

	.Lps4PayloadJumpHookStage1SkipCall:

	# --- move stuff back from arguments
	pushq 96(%r12)
	movq 104(%r12), %r11 # --- r12 saved to argument
	xchg %r11, %r12 # -- reset r12 keep args as r11

	push %r12
	lock decq 80(%r11) # --- decrease entry count
	movq $0, %r12
	lock xchg %r12, 64(%r11) # --- unlock
	pop %r12

	# --- ... profit ... *nyan*
	ret

.Lps4PayloadJumpHookStage1End:
.popsection

.pushsection .rodata

.global ps4PayloadJumpHookStage1Size
.type ps4PayloadJumpHookStage1Size, @object
ps4PayloadJumpHookStage1Size:
.int .Lps4PayloadJumpHookStage1End - ps4PayloadJumpHookStage1
.size ps4PayloadJumpHookStage1Size, .-ps4PayloadJumpHookStage1Size

.popsection
