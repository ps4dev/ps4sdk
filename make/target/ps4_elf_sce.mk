###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

include $(MakePath)/trait/sce.mk
include $(MakePath)/trait/base.mk
include $(MakePath)/trait/syscall_rop_standard.mk

###################################

all:: $(OutPath)/$(TargetFile)

###################################
