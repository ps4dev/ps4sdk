# VrR-jlDe7vs !

.pushsection .text

.global ps4PayloadTruth
.type ps4PayloadTruth, @function
ps4PayloadTruth:
	mov $1, %rax
	ret
.Lps4PayloadTruthEnd:
.size ps4PayloadTruth, .-ps4PayloadTruth

.popsection

.pushsection .rodata

.global ps4PayloadTruthSize
.type ps4PayloadTruthSize, @object
ps4PayloadTruthSize:
.int .Lps4PayloadTruthEnd - ps4PayloadTruth
.size ps4PayloadTruthSize, .-ps4PayloadTruthSize

.popsection
