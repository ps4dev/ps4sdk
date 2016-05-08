###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

include $(MakePath)/trait/common_extension.mk
include $(MakePath)/trait/kernel_execute.mk
include $(MakePath)/trait/common.mk
include $(MakePath)/trait/mixed.mk
include $(MakePath)/trait/sce.mk
include $(MakePath)/trait/kern.mk
include $(MakePath)/trait/base.mk
include $(MakePath)/trait/syscall_rop_0x93a4FFFF8.mk

###################################

all:: $(OutPath)/$(TargetFile)

###################################
