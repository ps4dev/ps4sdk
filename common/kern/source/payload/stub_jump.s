.pushsection .text

.global ps4PayloadStubJump64
.type ps4PayloadStubJump64, @function
ps4PayloadStubJump64:
	movabs $0, %r11
	jmp *%r11
.Lps4PayloadStubJumpEnd:
.size ps4PayloadStubJump64, .-ps4PayloadStubJump64

.global ps4PayloadStubJump32
.type ps4PayloadStubJump32, @function
ps4PayloadStubJump32:
	mov $0xdeadbeef, %r11d
	jmp *%r11
.Lps4PayloadStubJump32End:
.size ps4PayloadStubJump32, .-ps4PayloadStubJump32

.popsection

.pushsection .rodata

.global ps4PayloadStubJump64Size
.type ps4PayloadStubJump64Size, @object
ps4PayloadStubJump64Size:
.int .Lps4PayloadStubJumpEnd - ps4PayloadStubJump64
.size ps4PayloadStubJump64Size, .-ps4PayloadStubJump64Size

.global ps4PayloadStubJump32Size
.type ps4PayloadStubJump32Size, @object
ps4PayloadStubJump32Size:
.int .Lps4PayloadStubJump32End - ps4PayloadStubJump32
.size ps4PayloadStubJump32Size, .-ps4PayloadStubJump32Size

.popsection
