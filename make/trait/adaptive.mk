###################################

#Link in all libC and libkernel symbols that have a kernel alter-ego
#Magically switch their meaning and resolve them dependent on
#which space the code runs in. This overrides prior lib symbols!
#Allows us to write and execute kernel payloads compiled in with userland code
Libraries += -lPs4LibCInternalAdaptive_stub
Libraries += -lPs4LibKernelAdaptive_stub

###################################
