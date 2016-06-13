#pragma once

#include <sys/types.h>
#include <sys/sysent.h>

int64_t ps4AssemblerSystemCall();

void ps4AssemblerRegisterParameterPush();
void ps4AssemblerRegisterParameterPop();
