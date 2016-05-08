###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pic.mk

###################################

include $(MakePath)/trait/common.mk
include $(MakePath)/trait/kern.mk
include $(MakePath)/trait/base.mk

###################################

AllTarget := $(OutPath)/$(TargetFile).a
include $(MakePath)/trait/all_and_clean.mk

###################################
