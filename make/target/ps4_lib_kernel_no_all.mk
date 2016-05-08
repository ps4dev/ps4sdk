###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pic.mk

###################################

include $(MakePath)/trait/common.mk
include $(MakePath)/trait/kern.mk
include $(MakePath)/trait/base.mk

###################################

all:: $(OutPath)/$(TargetFile).a

###################################
