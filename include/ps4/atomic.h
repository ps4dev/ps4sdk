#pragma once

#include <stdint.h>

void ps4AtomicSwap64(uint64_t *atomic, uint64_t *swap);
void ps4AtomicSwap32(uint32_t *atomic, uint32_t *swap);
void ps4AtomicSwap16(uint16_t *atomic, uint16_t *swap);
void ps4AtomicSwap8(uint8_t *atomic, uint8_t *swap);

int ps4AtomicSpinLock64(uint64_t *atomic);
int ps4AtomicSpinUnlock64(uint64_t *atomic);
int ps4AtomicSpinLock32(uint32_t *atomic);
int ps4AtomicSpinUnlock32(uint32_t *atomic);
int ps4AtomicSpinLock16(uint16_t *atomic);
int ps4AtomicSpinUnlock16(uint16_t *atomic);
int ps4AtomicSpinLock8(uint8_t *atomic);
int ps4AtomicSpinUnlock8(uint8_t *atomic);
