#pragma once

#include "Memory.h"
#include <cstdint>

class CPU
{
private:
    uint16_t instruction = 0;

    void fetch();
    void decode();
    void execute();

    void ADD();
    void SUB();
    void AND();
    void OR();

public:
    uint16_t A = 0;
    uint16_t B = 0;
    uint16_t PC = 0;

    bool Z = false;
    bool N = false;
    bool C = false;

    Memory memory;

    void clock();
};