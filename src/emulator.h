//
// Created by jon on 6/29/17.
//

#ifndef CHIP8EMULATOR_EMULATOR_H
#define CHIP8EMULATOR_EMULATOR_H

#include <fstream>
#include "instruction.h"
#include "display.h"

typedef unsigned char byte;

// main class for the emulator, in charge of loading the rom and executing the code

class emulator {

private:
    // struct to hold the rom data
    struct rom_t {
        std::ifstream rom_stream;
        long size;
        byte *contents;

    };

    // actual rom var
    struct rom_t rom;

    // registers and related memory for the chip 8 system
    byte memory[4096];
    byte registers[16];
    byte delay_timer;
    byte sound_timer;
    unsigned short reg_I;
    unsigned short PC;
    byte SP;
    unsigned short stack[16];

    // display object
    display dis;

    // --- FUNCTIONS ---

    // clear the memory and register vars to zero or equiv
    void clearMemory();

    // execute the passed instruction
    void processInstruction(struct instruction_t instr);

    void processDisplayInstr(instruction_t instr);


public:
    // load contents of the rom described by "name" into the rom struct
    void loadRom(std::string name);

    // print the rom contents to the screen
    // (usually used to tell if the program read the file correctly
    void printRomContents();

    // execute the rom
    void run();

    void initializeDisplay();

    // load the predefined sprites into system memory
    void loadSprites();
};

void emulator::loadSprites() {
    // 0
    // 1
    // 2
    // 3
    // 4
    // 5
    // 6
    // 7
    // 8
    // 9
    // A
    // B
    // C
    // D
    // E
    // F
}


#endif //CHIP8EMULATOR_EMULATOR_H
