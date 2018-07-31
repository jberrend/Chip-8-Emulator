//
// Created by jon on 6/29/17.
//

#ifndef CHIP8EMULATOR_INSTRUCTION_H
#define CHIP8EMULATOR_INSTRUCTION_H

#include "emulator.hpp"

typedef unsigned char byte;

struct instruction_t {
    byte left_byte;
    byte right_byte;
    byte x; // -X--
    byte y; // --Y-
    unsigned short whole_instr;
};

#endif //CHIP8EMULATOR_INSTRUCTION_H
