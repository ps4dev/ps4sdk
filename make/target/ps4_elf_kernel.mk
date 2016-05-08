###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

include $(MakePath)/trait/common.mk
include $(MakePath)/trait/kern.mk
include $(MakePath)/trait/base.mk

###################################

all:: $(OutPath)/$(TargetFile)

###################################
