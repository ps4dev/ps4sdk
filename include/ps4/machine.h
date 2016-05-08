#pragma once

#include <stddef.h>

int ps4MachineInstructionNext(void *base, size_t *next);
int ps4MachineInstructionSeek(void *base, size_t *offset);
