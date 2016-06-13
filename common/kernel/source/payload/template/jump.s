.pushsection .text

.global ps4PayloadTemplateJump64
.type ps4PayloadTemplateJump64, @function
ps4PayloadTemplateJump64:
	movabs $0, %r11
	jmp *%r11
.Lps4PayloadTemplateJump64End:
.size ps4PayloadTemplateJump64, .-ps4PayloadTemplateJump64

.global ps4PayloadTemplateJump32
.type ps4PayloadTemplateJump32, @function
ps4PayloadTemplateJump32:
	movl $0, %r11d
	jmp *%r11
.Lps4PayloadTemplateJump32End:
.size ps4PayloadTemplateJump32, .-ps4PayloadTemplateJump32

.popsection

.pushsection .rodata

.global ps4PayloadTemplateJump64Size
.type ps4PayloadTemplateJump64Size, @object
ps4PayloadTemplateJump64Size:
.int .Lps4PayloadTemplateJump64End - ps4PayloadTemplateJump64
.size ps4PayloadTemplateJump64Size, .-ps4PayloadTemplateJump64Size

.global ps4PayloadTemplateJump32Size
.type ps4PayloadTemplateJump32Size, @object
ps4PayloadTemplateJump32Size:
.int .Lps4PayloadTemplateJump32End - ps4PayloadTemplateJump32
.size ps4PayloadTemplateJump32Size, .-ps4PayloadTemplateJump32Size

.popsection
