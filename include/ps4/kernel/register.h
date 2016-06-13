#pragma once

int ps4KernelRegisterControlSet(unsigned int c, unsigned long value);
int ps4KernelRegisterControlGet(unsigned int c, unsigned long *value);

void ps4KernelRegisterModelSpecificSet(unsigned int msr, uint64_t value);
int ps4KernelRegisterModelSpecificGet(unsigned int msr, uint64_t *value);
