//
// Created by jon on 6/29/17.
//

#ifndef CHIP8EMULATOR_EMULATOR_H
#define CHIP8EMULATOR_EMULATOR_H

#include <fstream>
#include "instruction.h"
#include "display.h"
#include "InputHandler.h"

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

    enum spriteLocations {
        zero  = 0x000,
        one   = 0x005,
        two   = 0x00A,
        three = 0x00F,
        four  = 0x014,
        five  = 0x019,
        six   = 0x01E,
        seven = 0x023,
        eight = 0x028,
        nine  = 0x02D,
        A     = 0x032,
        B     = 0x037,
        C     = 0x03C,
        D     = 0x041,
        E     = 0x046,
        F     = 0x04B
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

    // input handler
    InputHandler inputHandler;

    // --- FUNCTIONS ---

    // clear the memory and register vars to zero or equiv
    void clearMemory();

    // execute the passed instruction
    void processInstruction(struct instruction_t instr);

    // helper functions for specific instructions
    void processDisplayInstr(instruction_t instr);
    void processSpriteLoadInstr(instruction_t instr);


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

    // halt execution of the emulator until a key is pressed (used for 0xFx0A instr)
    byte waitForKeyPress();


};


#endif //CHIP8EMULATOR_EMULATOR_H
