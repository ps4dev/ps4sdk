###################################

include $(MakePath)/trait/freestanding.mk
include $(MakePath)/trait/pie.mk
include $(MakePath)/trait/link.mk

###################################

CrtFile ?= $(Ps4Sdk)/crt0.s
#CrtFile ?= crt0/crt0.s
#link = $(Linker) $? $(LinkerFlags) $(Libraries) -o $@

###################################

include $(MakePath)/trait/all_and_clean.mk

###################################
