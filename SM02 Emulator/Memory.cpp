#include "Memory.h"

uint16_t Memory::read(uint16_t address) const {
    return data[address];
}

void Memory::write(uint16_t address, uint16_t value) {
    data[address] = value;
}