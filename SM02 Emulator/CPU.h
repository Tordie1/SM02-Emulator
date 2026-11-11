#pragma once

#include "Memory.h"
#include <cstdint>
#include <unordered_map>
#include <array>
#include <mutex>

enum class CPUState
{
    fetch,
    execute,
    halted
};

class CPU {
private:
    void NOP();
    void OUT();
    void HLT();
    // Immediate instructions
    void LDA_val_imm();
    void LDB_val_imm();
    void LDA_addr_imm();
    void LDB_addr_imm();
    void STA_addr_imm();
    void STB_addr_imm();
    // Memory instructions
    void LDA_val_mem();
    void LDB_val_mem();
    void LDA_addr_mem();
    void LDB_addr_mem();
    void STA_addr_mem();
    void STB_addr_mem();
    // Pointer instructions
    void LDA_PTR();
    void STA_PTR();
    void LDP();
    void PTA();
    void PSH_A();
    void POP_A();
    void PSH_B();
    void POP_B();
    // Arithmetic
    void ADD();
    void SUB();
    void MUL();
    void DIV();
    void REM();
    // Bitwise
    void AND();
    void OR();
    void XOR();
    void NOT();
    // Shifts
    void SHL();
    void SHR();
    void ROL();
    void ROR();
    // Control flow
    void CMP();
    void JMP();
    void JE();
    void JNE();
    void JG();
    void JL();
    void JGE();
    void JLE();
    void JC();
    void JV();
    void CALL();
    void RET();

    void fetch();
    void execute();

    uint8_t opcode = 0;
    uint16_t immVal = 0;

    uint8_t instructionStep = 0;
    uint16_t MAR = 0; // Memory Address Register
    uint16_t ALUA = 0; // ALU a
    uint16_t ALUB = 0; // ALU b
    uint16_t ALUR = 0; // ALU remainder
    uint16_t ALUQ = 0; // ALU quotient

public:
    CPU();

    std::mutex cpuMutex;

    CPUState state = CPUState::fetch;
    Memory memory;

    std::array<uint16_t, 64> screen{};

    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t pc = 0;
    uint16_t i = 0;
    uint16_t ptr = 0;
    uint8_t sp = 0;
    uint16_t spOffset = 0xFF00;

    bool z = false;
    bool n = false;
    bool c = false;
    bool v = false;

    using Instruction = void (CPU::*)();

    std::unordered_map<uint8_t, Instruction> instructions;

    void clockPulse();
};