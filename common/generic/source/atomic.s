.pushsection .text

.global ps4AtomicSwap64
.type ps4AtomicSwap64, @function
ps4AtomicSwap64:
	movq (%rdi), %rax
	lock xchgq %rax, (%rsi)
	movq %rax, (%rdi)
	retq
.size ps4AtomicSwap64, .-ps4AtomicSwap64

.global ps4AtomicSwap32
.type ps4AtomicSwap32, @function
ps4AtomicSwap32:
	movl (%rdi), %eax
	lock xchgl %eax, (%rsi)
	movl %eax, (%rdi)
	retq
.size ps4AtomicSwap32, .-ps4AtomicSwap32

.global ps4AtomicSwap16
.type ps4AtomicSwap16, @function
ps4AtomicSwap16:
	xorl %eax, %eax
	movw (%rdi), %ax
	lock xchgw %ax, (%rsi)
	movw %ax, (%rdi)
	retq
.size ps4AtomicSwap16, .-ps4AtomicSwap16

.global ps4AtomicSwap8
.type ps4AtomicSwap8, @function
ps4AtomicSwap8:
	xorl %eax, %eax
	movb (%rdi), %al
	lock xchgb %al, (%rsi)
	movb %al, (%rdi)
	retq
.size ps4AtomicSwap8, .-ps4AtomicSwap8

.popsection
