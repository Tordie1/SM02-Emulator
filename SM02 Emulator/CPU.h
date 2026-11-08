#pragma once

#include "Memory.h"
#include <cstdint>
#include <unordered_map>

enum class CPUState
{
    fetch,
    execute
};

class CPU {
private:
    void NOP();
    void OUT();
    void HLT();
    void LDA_val_imm();
    void LDB_val_imm();
    void LDA_addr_imm();
    void LDB_addr_imm();
    void STA_addr_imm();
    void STB_addr_imm();

    CPUState state = CPUState::fetch;

    void fetch();
    void execute();

    uint8_t opcode = 0;
    uint8_t immVal = 0;

    uint8_t instructionStep = 0;
    uint16_t mar = 0; // Memory Address Register

public:
    CPU();

    Memory memory;

    std::array<uint16_t, 64> screen{};

    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t pc = 0;
    uint16_t i = 0;
    uint16_t ptr = 0;
    uint8_t sp = 0;

    bool z = false;
    bool n = false;
    bool c = false;
    bool v = false;

    bool halted = false;

    using Instruction = void (CPU::*)();

    std::unordered_map<uint8_t, Instruction> instructions;

    void clockPulse();
};