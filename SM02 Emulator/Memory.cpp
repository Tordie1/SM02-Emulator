#include "Memory.h"
#include <iostream>

uint16_t Memory::read(uint16_t address) const {
    return data[address];
}

void Memory::write(uint16_t address, uint16_t value) {
    data[address] = value;
}

void Memory::dump(bool skipZeroes) const {
    for (int i = 0; i < 65536; i++) {
        if (skipZeroes == true && data[i] == 0) {
            continue;
        }
        std::cout << "Index " << i << ": " << data[i] << "\n";
    }
}