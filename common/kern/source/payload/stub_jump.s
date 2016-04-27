.pushsection .text

.global ps4PayloadStubJump
.type ps4PayloadStubJump, @function
ps4PayloadStubJump:
	movabs $0, %r11
	jmp *%r11
.Lps4PayloadStubJumpEnd:
.size ps4PayloadStubJump, .-ps4PayloadStubJump

.popsection

.pushsection .rodata

.global ps4PayloadStubJumpSize
.type ps4PayloadStubJumpSize, @object
ps4PayloadStubJumpSize:
.int .Lps4PayloadStubJumpEnd - ps4PayloadStubJump
.size ps4PayloadStubJumpSize, .-ps4PayloadStubJumpSize

.popsection
