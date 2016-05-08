###################################

#Assembler syscalls are performed using the rop address stored at 0x...
#Needed for resolving stubs.
#This makes the elf dependant on a specific in-process setup.
Libraries += -lPs4_base_assembler_syscall_rop_0x93a4FFFF8

###################################
