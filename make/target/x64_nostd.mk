###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

#CrtFile ?= crt0/crt0.s
CrtFile ?= $(Ps4Sdk)/crt0.s
#link = $(Linker) $? $(LinkerFlags) $(Libraries) -o $@

###################################

all:: $(OutPath)/$(TargetFile)

###################################
