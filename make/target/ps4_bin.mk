###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk

###################################

include $(MakePath)/trait/kernel_execute.mk
include $(MakePath)/trait/common.mk
include $(MakePath)/trait/adaptive.mk
include $(MakePath)/trait/sce.mk
include $(MakePath)/trait/kernel.mk
include $(MakePath)/trait/base.mk
include $(MakePath)/trait/system_call_rop_0x93a4FFFF8.mk

###################################

ifndef KeepElf
ifdef keepelf
KeepElf := $(keepelf)
endif
ifdef KEEPELF
KeepElf := $(KEEPELF)
endif
endif

###################################

LinkerFlags += -Xlinker -T $(Ps4Sdk)/linker.x -Wl,--build-id=none

###################################

bincopy = $(ObjectCopy) $@ -O binary $@
copy = cp $@ $@.elf

###################################

$(OutPath)/$(TargetFile):: $(ObjectFiles)
	$(dirp)
	$(link)
ifdef KeepElf
	$(copy)
endif
	$(bincopy)

###################################

include $(MakePath)/trait/all_and_clean.mk

###################################
