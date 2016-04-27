###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

all:: $(OutPath)/$(TargetFile)

###################################
