.pushsection .text

# --- The lie is a condition of life.

.global ps4PayloadTemplateSystemCallHookPrologue
.type ps4PayloadTemplateSystemCallHookPrologue, @function
ps4PayloadTemplateSystemCallHookPrologue:

	# --- fix in args
	movabs $0, %r11

	# --- entry count
	lock incq 24(%r11)

	# --- spin lock
	push %r12
	.Lps4PayloadTemplateSystemCallHookPrologueLoop:
		movq $1, %r12
		lock xchg %r12, 40(%r11)
		testq %r12, %r12
		jnz .Lps4PayloadTemplateSystemCallHookPrologueLoop
	pop %r12

	# --- call clone
	movq %rsi, %rdx
	movq %r11, %rsi
	pushq %r11
	call *0(%r11)
	popq %r11

	# --- call handler args
	movq 16(%rax), %rdi
	movq %rax, %rsi

	# --- inc call count
	lock incq 32(%r11)

	# --- entry count - FIXME: should be in handler -> races free
	push %r12
	lock decq 24(%r11) # --- decrease entry count
	movq $0, %r12
	lock xchg %r12, 40(%r11) # --- unlock
	pop %r12

	# --- call handler
	jmpq *8(%rax)

.Lps4PayloadTemplateSystemCallHookPrologueEnd:
.size ps4PayloadTemplateSystemCallHookPrologue, .-ps4PayloadTemplateSystemCallHookPrologue

.popsection

.pushsection .rodata

.global ps4PayloadTemplateSystemCallHookPrologueSize
.type ps4PayloadTemplateSystemCallHookPrologueSize, @object
ps4PayloadTemplateSystemCallHookPrologueSize:
.int .Lps4PayloadTemplateSystemCallHookPrologueEnd - ps4PayloadTemplateSystemCallHookPrologue
.size ps4PayloadTemplateSystemCallHookPrologueSize, .-ps4PayloadTemplateSystemCallHookPrologueSize

.popsection
