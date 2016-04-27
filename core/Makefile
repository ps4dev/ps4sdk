###################################

Ps4Sdk=$(CURDIR)/..

target := ps4_lib_no_all
BuildPath := $(Ps4Sdk)/build
OutPath := $(Ps4Sdk)/lib

###################################

ifndef Ps4SdkFlags
ifdef ps4sdkflags
Ps4SdkFlags := $(ps4sdkflags)
endif
ifdef PS4SDKCFLAGS
Ps4SdkFlags := $(PS4SDKCFLAGS)
endif
endif

###################################

include $(Ps4Sdk)/make/ps4sdk.mk

#dirp = @mkdir -p $(@D)
archive := $(Archiver) $(ArchiverFlags)

$(BuildPath)/%.o: $(SourcePath)/%.s
	$(dirp)
	$(assemble)

#generateObject <type> <lib> <module> <symbol>
define generateObject
$(BuildPath)/$(strip $(2))/$(strip $(4)).stub.o: stub/$(strip $(1)).stub.c
ifeq ("$(wildcard $$@)", "")
	@mkdir -p $$@
	@rmdir $$@
endif
	$$(compile) -DPS4_STUB_MODULE="$(strip $(3))" -DPS4_STUB_SYMBOL="$(strip $(4))"

endef

#generateLibrary <type> <lib> <module> <symbols> <extobj>
define generateLibrary
$(foreach i, $(4), $(call generateObject, $(1), $(2), $(3), $(i)))
$(OutPath)/$(strip $(2))_stub.a: $(5) $(patsubst %,$(BuildPath)/$(strip $(2))/%.stub.o,$(strip $(4)))
	$$(dirp)
	$$(archive) $$@ $$?
all:: $(OutPath)/$(strip $(2))_stub.a

endef

#generateModule <module> <symbols>
define generateModule
$(call generateObject, module, $(1), $(1), $(1))
$(call generateLibrary, function, $(1), $(1), $(2), $(BuildPath)/$(strip $(1))/$(strip $(1)).stub.o)

endef

include $(CURDIR)/stub.mk

###################################
