#pragma once

#include <cstdint>
#include <array>

class Memory {
private:
    std::array<uint16_t, 65536> data{};

public:
    uint16_t read(uint16_t address) const;
    void write(uint16_t address, uint16_t value);
    void dump(bool skipZeroes = true) const;
};