#include <cstdint>
#include "CPU.h"

uint16_t A = 0;
uint16_t B = 0;
uint16_t PC = 0;

uint8_t opcode = 0;
bool zeroFlag = false;
bool carryFlag = false;
bool negativeFlag = false;

void CPU::fetch()
{
    CPU::instruction = Memory.read(CPU::PC);
}

void CPU::clock()
{
    fetch();
    decode();
    execute();
}