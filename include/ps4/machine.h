#pragma once

#include <stddef.h>

int ps4MachineInstructionNext(void *current, void **next);
int ps4MachineInstructionSeek(void *current, void **found, size_t offset);
