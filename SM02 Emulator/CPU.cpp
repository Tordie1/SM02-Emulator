#include <cstdint>
#include "CPU.h"
#include <iostream>
#include <bitset>

void CPU::NOP() {
    std::cout << "NOP\n";
    pc++;
    state = CPUState::fetch;
}
void CPU::OUT() {

}
void CPU::HLT() {

}
void CPU::LDA_val_imm() {

}
void CPU::LDB_val_imm() {

}
void CPU::LDA_addr_imm() {

}
void CPU::LDB_addr_imm() {

}
void CPU::STA_addr_imm() {

}
void CPU::STB_addr_imm() {

}

CPU::CPU() {
    instructions = {
        { 0x00, &CPU::NOP },
        { 0x3E, &CPU::OUT },
        { 0x3F, &CPU::HLT },
        { 0x01, &CPU::LDA_val_imm },
        { 0x02, &CPU::LDB_val_imm },
        { 0x03, &CPU::LDA_addr_imm },
        { 0x04, &CPU::LDB_addr_imm },
        { 0x05, &CPU::STA_addr_imm },
        { 0x06, &CPU::STB_addr_imm }
    };
}

void CPU::fetch() {
    switch (instructionStep) {
    case 0: // Fetch instruction address
        std::cout << "fetch instruction address\n";
        mar = pc;
        pc++;
        instructionStep++;
        break;
    case 1: // Load instruction into instruction register
        std::cout << "load instruction\n";
        i = memory.read(mar);
        opcode = (i && 64512) >> 10; // First 6 bits
        immVal = (i && 1023); // Last 10 bits
        std::cout << "instruction: " << std::bitset<16>(i) << "\n";
        std::cout << "opcode: " << std::bitset<16>(opcode) << "\n";
        std::cout << "immVal: " << std::bitset<16>(immVal) << "\n";
        instructionStep = 0;
        state = CPUState::execute;
        break;
    }
}

void CPU::execute() {
    std::cout << "execute\n";
    auto it = instructions.find(opcode);

    if (it != instructions.end())
    {
        (this->*(it->second))();
    }
}

void CPU::clockPulse() {
    switch (state) {
        case CPUState::fetch:
            fetch();
            break;
        case CPUState::execute:
            execute();
            break;
    }
}