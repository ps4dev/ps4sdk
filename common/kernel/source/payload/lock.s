.pushsection .text

.global ps4PayloadLock
.type ps4PayloadLock, @function
ps4PayloadLock:
	jmp .-2
.Lps4PayloadLockEnd:
.size ps4PayloadLock, .-ps4PayloadLock

.popsection

.pushsection .rodata

.global ps4PayloadLockSize
.type ps4PayloadLockSize, @object
ps4PayloadLockSize:
.int .Lps4PayloadLockEnd - ps4PayloadLock
.size ps4PayloadLockSize, .-ps4PayloadLockSize

.popsection
