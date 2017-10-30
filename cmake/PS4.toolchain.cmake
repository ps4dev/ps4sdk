set(triple "x86_64-scei-ps4")

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER_AR "ar")
set(CMAKE_LINKER "clang")
set(CMAKE_ASM_COMPILER "clang")
set(CMAKE_FIND_ROOT_PATH "$ENV{PS4SDK}")
set(CMAKE_SYSROOT "$ENV{PS4SDK}/")

# compiler has to be 'forced' to skip cxx compiler test
# which fails due the crt0.s not being compatible with CXX
set(CMAKE_CXX_COMPILER_FORCED on)
set(CMAKE_C_COMPILER_TARGET "${triple}")
set(CMAKE_CXX_COMPILER_TARGET "${triple}")

# only search for libraries and includes in the PS4SDK directory
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_SKIP_RPATH true)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" CACHE STRING "")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" CACHE STRING "")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -pedantic -m64 -mcmodel=large -nostdlib -nostdinc -ffreestanding -fPIE -fno-builtin -fno-stack-protector -D__PS4__" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -m64 -mcmodel=large -nostdlib -nostdinc -ffreestanding -fPIE -fno-stack-protector -D__PS4__" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "-O3 -Wall -m64 -pie -nostartfiles -nostdlib  $ENV{PS4SDK}/crt0.s " CACHE STRING "")

link_directories("$ENV{PS4SDK}/lib")

set(TOOLCHAIN_PS4SDK_LIBS)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_extension_kernel_execute_dynlib_prepare_dlclose Ps4_extension_kernel_call_standard)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_common_kernel Ps4_common_user Ps4_common_generic)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_common_generic)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4LibCInternalAdaptive_stub Ps4LibKernelAdaptive_stub)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS SceLibcInternal_stub kernel_stub)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS ps4Kernel_stub)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_base_stub_resolve_minimal)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_base_kernel_dlsym_standard Ps4_base_kernel_seek_elf_address_standard)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_base_assembler_register_parameter_standard)
LIST(APPEND TOOLCHAIN_PS4SDK_LIBS Ps4_base_assembler_system_call_rop_0x93a4FFFF8)


# this is a work around to avoid duplicate library warning,
# as the toolchain file can be proccessed multiple times
# people over cmake IRC suggest I move this to a find module
# instead of messing with policies as they might be removed in time AKA TODO
cmake_policy(SET CMP0002 OLD)

add_library(PS4LIBS INTERFACE)
add_library(PS4::LIBS ALIAS PS4LIBS)
target_link_libraries(PS4LIBS INTERFACE ${TOOLCHAIN_PS4SDK_LIBS})

add_library(PS4HEADERS INTERFACE)
add_library(PS4::HEADERS ALIAS PS4HEADERS)
target_include_directories(PS4HEADERS INTERFACE "$ENV{PS4SDK}/include/c++/v1/" "$ENV{PS4SDK}/include/" "$ENV{PS4SDK}/include/sce/" "$ENV{PS4SDK}/include/ps4/")
include_directories("$ENV{PS4SDK}/include/c++/v1/" "$ENV{PS4SDK}/include/" "$ENV{PS4SDK}/include/sce/" "$ENV{PS4SDK}/include/ps4/")

cmake_policy(SET CMP0002 NEW)
