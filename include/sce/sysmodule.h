#pragma once

#include <stdint.h>

int sceSysmoduleIsLoaded(uint16_t id);
int sceSysmoduleLoadModule(uint16_t id);
int sceSysmoduleUnloadModule(uint16_t id);
