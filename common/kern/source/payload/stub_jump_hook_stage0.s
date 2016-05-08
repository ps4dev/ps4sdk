.pushsection .text

# --- The lie is a condition of life.

.global ps4PayloadStubJumpHookStage0
.type ps4PayloadStubJumpHookStage0, @function
ps4PayloadStubJumpHookStage0:

	# --- fix in args & count & jump
	movabs $0, %r11 # --- arguments
	nop
	nop # --- potential for safe jump redirection
	nop

	# --- entry count
	lock incq 80(%r11)

	# --- spin lock
	push %r12
	.Lps4PayloadStubJumpHookStage0Loop:
		movq $1, %r12
		lock xchg %r12, 64(%r11)
		testq %r12, %r12
		jnz .Lps4PayloadStubJumpHookStage0Loop
	pop %r12

	jmpq *112(%r11) # --- stage 1, fixed value

.Lps4PayloadStubJumpHookStage0End:
.size ps4PayloadStubJumpHookStage0, .-ps4PayloadStubJumpHookStage0

.popsection

.pushsection .rodata

.global ps4PayloadStubJumpHookStage0Size
.type ps4PayloadStubJumpHookStage0Size, @object
ps4PayloadStubJumpHookStage0Size:
.int .Lps4PayloadStubJumpHookStage0End - ps4PayloadStubJumpHookStage0
.size ps4PayloadStubJumpHookStage0Size, .-ps4PayloadStubJumpHookStage0Size

.popsection
