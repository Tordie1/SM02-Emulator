#include <cstdint>
#include "CPU.h"
#include <iostream>
#include <bitset>

void CPU::NOP() {
    state = CPUState::fetch;
}
void CPU::OUT() {
    screen[b] = a;
    state = CPUState::fetch;
}
void CPU::HLT() {
    state = CPUState::halted;
}
// <=== Immediate instructions ===>
void CPU::LDA_val_imm() {
    a = immVal;
    state = CPUState::fetch;
}
void CPU::LDB_val_imm() {
    b = immVal;
    state = CPUState::fetch;
}
void CPU::LDA_addr_imm() {
    switch (instructionStep) {
        case 0:
            MAR = immVal;
            instructionStep++;
            break;
        case 1:
            a = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::LDB_addr_imm() {
    switch (instructionStep) {
        case 0:
            MAR = immVal;
            instructionStep++;
            break;
        case 1:
            b = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::STA_addr_imm() {
    switch (instructionStep) {
        case 0:
            MAR = immVal;
            instructionStep++;
            break;
        case 1:
            memory.write(MAR, a);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::STB_addr_imm() {
    switch (instructionStep) {
        case 0:
            MAR = immVal;
            instructionStep++;
            break;
        case 1:
            memory.write(MAR, b);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
// <=== Memory instructions ===>
void CPU::LDA_val_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            a = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}                 
void CPU::LDB_val_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            b = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::LDA_addr_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            MAR = memory.read(MAR);
            instructionStep++;
            break;
        case 2:
            a = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::LDB_addr_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            MAR = memory.read(MAR);
            instructionStep++;
            break;
        case 2:
            b = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::STA_addr_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            MAR = memory.read(MAR);
            instructionStep++;
            break;
        case 2:
            memory.write(MAR, a);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::STB_addr_mem() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            pc++;
            break;
        case 1:
            MAR = memory.read(MAR);
            instructionStep++;
            break;
        case 2:
            memory.write(MAR, b);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
// <=== Pointer instructions ===>
void CPU::LDA_PTR() {
    switch (instructionStep) {
        case 0:
            MAR = ptr;
            instructionStep++;
            break;
        case 1:
            a = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::STA_PTR() {
    switch (instructionStep) {
        case 0:
            MAR = ptr;
            instructionStep++;
            break;
        case 1:
            memory.write(MAR, a);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::LDP() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            pc++;
            instructionStep++;
            break;
        case 1:
            ptr = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::PTA() {
    ptr += b;
    state = CPUState::fetch;
}
// For some reason, I messed up the PSH instructions
// in the CPU, and the SP gets incremented before the
// value gets stored, and vice versa for POP.
void CPU::PSH_A() {
    switch (instructionStep) {
        case 0:
            sp++;
            instructionStep++;
            break;
        case 1:
            memory.write(sp + spOffset, a);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::POP_A() {
    switch (instructionStep) {
        case 0:
            a = memory.read(sp + spOffset);
            instructionStep++;
            break;
        case 1:
            sp--;
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::PSH_B() {
    switch (instructionStep) {
        case 0:
            sp++;
            instructionStep++;
            break;
        case 1:
            memory.write(sp + spOffset, b);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::POP_B() {
    switch (instructionStep) {
        case 0:
            b = memory.read(sp + spOffset);
            instructionStep++;
            break;
        case 1:
            sp--;
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
// <=== Arithmetic ===>
void CPU::ADD() {
    uint32_t result = static_cast<uint32_t>(a) + static_cast<uint32_t>(b);

    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    bool rSign = result >> 15 & 1 ? true : false;

    z = static_cast<uint16_t>(result) == 0 ? true : false;
    n = result >> 15 & 1 ? true : false;
    c = result > std::numeric_limits<uint16_t>::max() ? true : false;
    v = !(aSign ^ bSign) && (aSign ^ rSign) ? true : false;
    // Signed carry (V) if the operands have the same sign
    // and the result has a different sign. This is right,
    // however the actual CPU has it wrong!
    // IDK if I should keep the error.
    a = result;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::SUB() {
    uint32_t result = static_cast<uint32_t>(a) + static_cast<uint32_t>(~b + 1);

    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    bool rSign = result >> 15 & 1 ? true : false;

    z = static_cast<uint16_t>(result) == 0 ? true : false;
    n = result >> 15 & 1 ? true : false;
    c = result > std::numeric_limits<uint16_t>::max() ? true : false;
    v = (aSign ^ bSign) && !(bSign ^ rSign) ? true : false;
    a = result;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::MUL() {
    uint32_t result = 0;
    // Im pretty sure the cumulative result is stored in ALUQ, but I can't check right now.
    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    if (instructionStep < 2) {
        switch (instructionStep) {
            case 0:
                ALUA = aSign ? ~a + 1 : a; // ABS of a
                instructionStep++;
                break;
            case 1:
                ALUB = bSign ? ~b + 1 : b; // ABS of b
                instructionStep++;
                break;
        }
        return;
    }
    if (instructionStep < 18) {
        bool lsbB = ALUB & 1 ? true : false;
        result = lsbB ? result + ALUA: result;
        ALUA = ALUA << 1;
        ALUB = ALUB >> 1;
        instructionStep++;
        return;
    }
    a = (aSign ^ bSign) ? ~result + 1 : result;
    state = CPUState::fetch;
    instructionStep = 0;
}
// DIV and REM are literally the same algorithm, it just writes
// either the quotient or remander. Optimization possible.
void CPU::DIV() {
    uint32_t temp = 0;
    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    if (instructionStep < 2) {
        switch (instructionStep) {
            case 0:
                ALUA = aSign ? ~a + 1 : a; // ABS of a
                instructionStep++;
                break;
            case 1:
                ALUB = bSign ? ~b + 1 : b; // ABS of b
                instructionStep++;
                break;
        }
        return;
    }
    // IDK how this works, it just does.
    if (instructionStep < 34) {
        bool evenStep = instructionStep & 1 ? true : false;
        switch (evenStep) {
            case true:
                ALUR = (ALUR << 1) + (ALUA >> 15);
                ALUA = ALUA << 1;
                ALUQ = ALUQ << 1;
                break;
            case false:
                temp = ALUR + (~ALUB + 1); // R - B, idk if I can just to ALUR - ALUB
                if (((temp >> 15) & 1) == 0) {
                    ALUR = temp;
                    ALUQ++;
                }
                break;
        }
        return;
    }
    a = (aSign ^ bSign) ? ~ALUQ + 1 : ALUQ;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::REM() {
    uint32_t temp = 0;
    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    if (instructionStep < 2) {
        switch (instructionStep) {
        case 0:
            ALUA = aSign ? ~a + 1 : a; // ABS of a
            instructionStep++;
            break;
        case 1:
            ALUB = bSign ? ~b + 1 : b; // ABS of b
            instructionStep++;
            break;
        }
        return;
    }
    // IDK how this works, it just does.
    if (instructionStep < 34) {
        bool evenStep = instructionStep & 1 ? true : false;
        switch (evenStep) {
            case true:
                ALUR = (ALUR << 1) + (ALUA >> 15);
                ALUA = ALUA << 1;
                ALUQ = ALUQ << 1;
                break;
            case false:
                temp = ALUR + (~ALUB + 1); // R - B, idk if I can just to ALUR - ALUB
                if (((temp >> 15) & 1) == 0) {
                    ALUR = temp;
                    ALUQ++;
                }
                break;
        }
        return;
    }
    a = (aSign ^ bSign) ? ~ALUR + 1 : ALUR;
    state = CPUState::fetch;
    instructionStep = 0;
}
// <=== Bitwise ===>
void CPU::AND() {
    a = a & b;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::OR() {
    a = a | b;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::XOR() {
    a = a ^ b;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::NOT() {
    a = ~a;
    state = CPUState::fetch;
    instructionStep = 0;
}
// <=== Shifts ===>
void CPU::SHL() {
    a = a << 1;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::SHR() {
    a = a >> 1;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::ROL() {
    a = (a << 1) | (a >> 15);
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::ROR() {
    a = (a >> 1) | (a << 15);
    state = CPUState::fetch;
    instructionStep = 0;
}
// <=== Control flow ===>
void CPU::CMP() { // Exactly just SUB, just doesn't write to a, could be optimized
    uint32_t result = static_cast<uint32_t>(a) + static_cast<uint32_t>(~b + 1);

    bool aSign = a >> 15 & 1 ? true : false;
    bool bSign = b >> 15 & 1 ? true : false;
    bool rSign = result >> 15 & 1 ? true : false;

    z = static_cast<uint16_t>(result) == 0 ? true : false;
    n = result >> 15 & 1 ? true : false;
    c = result > std::numeric_limits<uint16_t>::max() ? true : false;
    v = (aSign ^ bSign) && !(bSign ^ rSign) ? true : false;
    state = CPUState::fetch;
    instructionStep = 0;
}
void CPU::JMP() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            pc = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JE() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (z == true) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JNE() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (z == false) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
        }
}
void CPU::JG() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if ((z == false) && (n == v)) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JL() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (n != v) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JGE() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (n == v) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JLE() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if ((z == 1) || (n != v)) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JC() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (c == 1) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::JV() {
    switch (instructionStep) {
        case 0:
            MAR = pc;
            instructionStep++;
            break;
        case 1:
            if (v == 1) {
                pc = memory.read(MAR);
            }
            else {
                pc++;
            }
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
// REMINDER -- Check these last two functions to make sure they're accurate
void CPU::CALL() {
    switch (instructionStep) {
        case 0:
            sp++;
            instructionStep++;
            break;
        case 1:
            memory.write(sp + spOffset, pc);
            instructionStep++;
            break;
        case 2:
            MAR = pc;
            instructionStep++;
            break;
        case 3:
            pc = memory.read(MAR);
            state = CPUState::fetch;
            instructionStep = 0;
    }
}
void CPU::RET() {
    switch (instructionStep) {
        case 0:
            MAR = sp;
            instructionStep++;
            break;
        case 1:
            pc = memory.read(sp + spOffset);
            instructionStep++;
            break;
        case 2:
            sp--;
            pc++;
            state = CPUState::fetch;
            instructionStep = 0;
    }
}

CPU::CPU() {
    instructions = {
        { 0x00, &CPU::NOP },
        { 0x3E, &CPU::OUT },
        { 0x3F, &CPU::HLT },
        // Immediate instructions
        { 0x01, &CPU::LDA_val_imm },
        { 0x02, &CPU::LDB_val_imm },
        { 0x03, &CPU::LDA_addr_imm },
        { 0x04, &CPU::LDB_addr_imm },
        { 0x05, &CPU::STA_addr_imm },
        { 0x06, &CPU::STB_addr_imm },
        // Memory instructions
        { 0x07, &CPU::LDA_val_mem },
        { 0x08, &CPU::LDB_val_mem },
        { 0x09, &CPU::LDA_addr_mem },
        { 0x0A, &CPU::LDB_addr_mem },
        { 0x0B, &CPU::STA_addr_mem },
        { 0x0C, &CPU::STB_addr_mem },
        // Pointer instructions
        { 0x0D, &CPU::LDA_PTR },
        { 0x0E, &CPU::STA_PTR },
        { 0x0F, &CPU::LDP },
        { 0x10, &CPU::PTA },
        { 0x11, &CPU::PSH_A },
        { 0x12, &CPU::POP_A },
        { 0x13, &CPU::PSH_B },
        { 0x14, &CPU::POP_B },
        // Arithmetic
        { 0x15, &CPU::ADD },
        { 0x16, &CPU::SUB },
        { 0x17, &CPU::MUL },
        { 0x18, &CPU::DIV },
        { 0x19, &CPU::REM },
        // Bitwise
        { 0x1A, &CPU::AND },
        { 0x1B, &CPU::OR },
        { 0x1C, &CPU::XOR },
        { 0x1D, &CPU::NOT },
        // Shifts
        { 0x1E, &CPU::SHL },
        { 0x1F, &CPU::SHR },
        { 0x20, &CPU::ROL },
        { 0x21, &CPU::ROR },
        // Control flow
        { 0x22, &CPU::CMP },
        { 0x23, &CPU::JMP },
        { 0x24, &CPU::JE },
        { 0x25, &CPU::JNE },
        { 0x26, &CPU::JG },
        { 0x27, &CPU::JL },
        { 0x28, &CPU::JGE },
        { 0x29, &CPU::JLE },
        { 0x2A, &CPU::JC },
        { 0x2B, &CPU::JV },
        { 0x2C, &CPU::CALL },
        { 0x2D, &CPU::RET },
    };
}

void CPU::fetch() {
    switch (instructionStep) {
    case 0: // Fetch instruction address
        //std::cout << "fetch instruction address\n";
        MAR = pc;
        pc++;
        instructionStep++;
        break;
    case 1: // Load instruction into instruction register
        //std::cout << "load instruction\n";
        i = memory.read(MAR);
        opcode = i >> 10; // 1111110000000000
        immVal = (i & ((1 << 10) - 1));  // 0000001111111111
        //std::cout << "instruction: " << std::bitset<16>(i) << "\n";
        //std::cout << "opcode: " << std::bitset<6>(opcode) << "----------\n";
        //std::cout << "immVal: ------" << std::bitset<10>(immVal) << "\n";
        instructionStep = 0;
        state = CPUState::execute;
        break;
    }
}

void CPU::execute() {
    //std::cout << "execute\n";
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
    }
}