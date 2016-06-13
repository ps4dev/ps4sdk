.pushsection .text

.global ps4PayloadTemplateReturn64 # --- int return
.type ps4PayloadTemplateReturn64, @function
ps4PayloadTemplateReturn64:
	movabs $0, %rax
	ret
.Lps4PayloadTemplateReturn64End:
.size ps4PayloadTemplateReturn64, .-ps4PayloadTemplateReturn64

.global ps4PayloadTemplateReturn32 # --- smaller
.type ps4PayloadTemplateReturn32, @function
ps4PayloadTemplateReturn32:
	movl $0, %eax
	ret
.Lps4PayloadTemplateReturn32End:
.size ps4PayloadTemplateReturn32, .-ps4PayloadTemplateReturn32

.global ps4PayloadTemplateReturnZero # --- smaller then return payload
.type ps4PayloadTemplateReturnZero, @function
ps4PayloadTemplateReturnZero:
	xorq %rax, %rax
	ret
.Lps4PayloadTemplateReturnZeroEnd:
.size ps4PayloadTemplateReturnZero, .-ps4PayloadTemplateReturnZero

# --- FIXME: add st0?

.popsection

.pushsection .rodata

.global ps4PayloadTemplateReturn64Size
.type ps4PayloadTemplateReturn64Size, @object
ps4PayloadTemplateReturn64Size:
.int .Lps4PayloadTemplateReturn64End - ps4PayloadTemplateReturn64
.size ps4PayloadTemplateReturn64Size, .-ps4PayloadTemplateReturn64Size

.global ps4PayloadTemplateReturn32Size
.type ps4PayloadTemplateReturn32Size, @object
ps4PayloadTemplateReturn32Size:
.int .Lps4PayloadTemplateReturn32End - ps4PayloadTemplateReturn32
.size ps4PayloadTemplateReturn32Size, .-ps4PayloadTemplateReturn32Size

.global ps4PayloadTemplateReturnZeroSize
.type ps4PayloadTemplateReturnZeroSize, @object
ps4PayloadTemplateReturnZeroSize:
.int .Lps4PayloadTemplateReturnZeroEnd - ps4PayloadTemplateReturnZero
.size ps4PayloadTemplateReturnZeroSize, .-ps4PayloadTemplateReturnZeroSize

.popsection
