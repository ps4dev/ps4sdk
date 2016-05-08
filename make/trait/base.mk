###################################

#Use minimal resolver to get the stubs auto-resolved.
Libraries += -lPs4_base_stub_resolve_minimal

#Be able to find kernel elf and by extension to ps4KernelDlSym
Libraries += -lPs4_base_kern_dlsym_standard
Libraries += -lPs4_base_kern_seek_elf_address_standard

#Ability to push / pop all arguments in registers from assembler
#Used in stub relove
Libraries += -lPs4_base_assembler_register_parameter_standard

###################################
