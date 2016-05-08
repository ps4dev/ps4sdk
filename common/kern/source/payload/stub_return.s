.pushsection .text

.global ps4PayloadStubReturn64 # --- int return
.type ps4PayloadStubReturn64, @function
ps4PayloadStubReturn64:
	movabs $0, %rax
	ret
.Lps4PayloadStubReturn64End:
.size ps4PayloadStubReturn64, .-ps4PayloadStubReturn64

.global ps4PayloadStubReturn32 # --- smaller
.type ps4PayloadStubReturn32, @function
ps4PayloadStubReturn32:
	mov $0, %eax
	ret
.Lps4PayloadStubReturn32End:
.size ps4PayloadStubReturn32, .-ps4PayloadStubReturn32

.global ps4PayloadStubReturnZero # --- smaller then return payload
.type ps4PayloadStubReturnZero, @function
ps4PayloadStubReturnZero:
	xorq %rax, %rax
	ret
.Lps4PayloadStubReturnZeroEnd:
.size ps4PayloadStubReturnZero, .-ps4PayloadStubReturnZero

# --- FIXME: add st0?

.popsection

.pushsection .rodata

.global ps4PayloadStubReturn64Size
.type ps4PayloadStubReturn64Size, @object
ps4PayloadStubReturn64Size:
.int .Lps4PayloadStubReturn64End - ps4PayloadStubReturn64
.size ps4PayloadStubReturn64Size, .-ps4PayloadStubReturn64Size

.global ps4PayloadStubReturn32Size
.type ps4PayloadStubReturn32Size, @object
ps4PayloadStubReturn32Size:
.int .Lps4PayloadStubReturn32End - ps4PayloadStubReturn32
.size ps4PayloadStubReturn32Size, .-ps4PayloadStubReturn32Size

.global ps4PayloadStubReturnZeroSize
.type ps4PayloadStubReturnZeroSize, @object
ps4PayloadStubReturnZeroSize:
.int .Lps4PayloadStubReturnZeroEnd - ps4PayloadStubReturnZero
.size ps4PayloadStubReturnZeroSize, .-ps4PayloadStubReturnZeroSize

.popsection
