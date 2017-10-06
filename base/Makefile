###################################

Ps4Sdk ?= $(CURDIR)/..

target ?= ps4_lib_no_all
BuildPath := $(Ps4Sdk)/build
OutPath := $(Ps4Sdk)/lib

###################################

include $(Ps4Sdk)/make/ps4sdk.mk

###################################

define generateCObject
$(patsubst %,$(BuildPath)/%,$(patsubst %.c,%.o,$(1))): $(1)
	$$(dirp)
	$$(compile)

endef

define generateSObject
$(patsubst %,$(BuildPath)/%,$(patsubst %.s,%.o,$(1))): $(1)
	$$(dirp)
	$$(assemble)

endef

define generateTarget
$(foreach i,$(wildcard $(1)source/*.c),$(call generateCObject,$(i)))
$(foreach i,$(wildcard $(1)source/*.s),$(call generateSObject,$(i)))

$(OutPath)/libPs4_base_$(subst /,_,$(patsubst %/,%,$(1))).a: $(patsubst %,$(BuildPath)/%, $(patsubst %.c,%.o,$(wildcard $(1)source/*.c)) $(patsubst %.s,%.o,$(wildcard $(1)source/*.s)))
	$$(dirp)
	$$(archive)
all:: $(OutPath)/libPs4_base_$(subst /,_,$(patsubst %/,%,$(1))).a
clean::
	rm -fR $(BuildPath) $(OutPath)/libPs4_base_$(subst /,_,$(patsubst %/,%,$(1))).a
	-rmdir -p $(BuildPath)/$(strip $(1))

endef

define generateTargets
$(foreach i,$(1),$(call generateTarget,$(i)))

endef

$(eval $(call generateTargets,$(wildcard */*/)))
