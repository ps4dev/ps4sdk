###################################

ifndef Ps4Sdk
ifdef ps4sdk
Ps4Sdk := $(ps4sdk)
endif
ifdef PS4SDK
Ps4Sdk := $(PS4SDK)
endif
ifndef Ps4Sdk
$(error Neither PS4SDK, Ps4Sdk nor ps4sdk set)
endif
endif

###################################

AssemblerFlags += -I$(Ps4Sdk)/include
CompilerFlags += -D__PS4__ -I$(Ps4Sdk)/include -I $(Ps4Sdk)/include/sce
LinkerFlags += -L$(Ps4Sdk)/lib

ifndef Libraries
#Default target to create previous "-lps4" elfs
#Note reverse order from high level libs to low level base

#Link in kernel exploit by default
#This makes the executable firmware dependent
Libraries += -lPs4_extension_kernel_execute_dynlib_prepare_dlclose

#Link in core
Libraries += -lPs4_common_mixed
Libraries += -lPs4_common_kern
Libraries += -lPs4_common_user

#Link in all libC and libkernel symbols that have a kernel alter-ego
#Magically switch their meaning and resolve them dependent on
#which space the code runs in. This overrides prior lib symbols!
#Allows us to write and execute kernel payloads compiled in with userland code
Libraries += -lPs4LibCInternalMixed_stub
Libraries += -lPs4LibKernelMixed_stub

#Include standard libc, posix and common process management stuff
Libraries += -lSceLibcInternal_stub
Libraries += -lkernel_stub

#Link in kernel stubs to resolve calls through ps4KernelExecute
Libraries += -lPs4Kern_stub

#Use minimal resolver to get the stubs auto-resolved.
Libraries += -lPs4_base_stub_resolve_minimal

#Be able to find kernel elf and by extension to ps4KernelDlSym
Libraries += -lPs4_base_kern_dlsym_standard
Libraries += -lPs4_base_kern_seek_elf_address_standard

#Assembler syscalls are performed using the rop address stored at 0x...
#Needed for resolving stubs.
#This makes the elf dependant on a specific in-process setup.
Libraries += -lPs4_base_assembler_syscall_rop_0x93a4FFFF8

#Ability to push / pop all argument registers
Libraries += -lPs4_base_assembler_register_standard
endif

###################################

CrtFile ?= $(Ps4Sdk)/crt0.s
#link = $(Linker) $(Ps4Sdk)/crt0.s $? $(LinkerFlags) $(Libraries) -o $@

###################################
