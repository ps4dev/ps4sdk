###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/ps4.mk

###################################

OutPath ?= lib

###################################

CompilerFlags += $(Ps4SdkFlags) -fPIC

###################################
