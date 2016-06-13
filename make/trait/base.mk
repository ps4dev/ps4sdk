###################################

#Use minimal resolver to get the stubs auto-resolved.
Libraries += -lPs4_base_stub_resolve_minimal

#Be able to find kernel elf and by extension to ps4KernelelDlSym
Libraries += -lPs4_base_kernel_dlsym_standard
Libraries += -lPs4_base_kernel_seek_elf_address_standard

#Ability to push / pop all arguments in registers from assembler
#Used in stub relove
Libraries += -lPs4_base_assembler_register_parameter_standard

###################################
