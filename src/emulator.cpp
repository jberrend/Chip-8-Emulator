//
// Created by jon on 6/29/17.
//

#include <chrono>
#include <thread>
#include "emulator.h"
#include "instruction.h"

void emulator::loadRom(std::string name) {
    // clear the memory vars for use
    this->clearMemory();

    // clear the display buffer
    dis.clearBuffer();

    // open the file for reading
    rom.rom_stream.open(name, std::ifstream::in | std::ifstream::binary);

    // get the size of the file
    rom.rom_stream.seekg(0, rom.rom_stream.end);
    rom.size = rom.rom_stream.tellg();
    rom.rom_stream.seekg(0, rom.rom_stream.beg); // back to the start

    // initialize the buffer to hold the contents of the rom file
    rom.contents = (byte*) malloc(sizeof(byte) * rom.size);
    for (int i = 0; i < rom.size; i++) {
        rom.contents[i] = 0;
    }

    // actually read the contents from the file
    rom.rom_stream.read((char *) rom.contents, rom.size);

    // done with the file
    rom.rom_stream.close();

    // load the rom contents into memory
    for (int i = 0; i < rom.size; i++) {
        memory[i + 0x200] = rom.contents[i];
    }

    // set the PC to point to the start of the program
    PC = 0x200;
}

void emulator::printRomContents() {
    for (int i = 0; i < rom.size; i++) {
        printf("0x%X ", rom.contents[i]);
    }
}

void emulator::clearMemory() {
    for (int i = 0x200; i < 4096; i++) {
        memory[i] = 0x00;
    }

    loadSprites();

    for (int i = 0; i < 8; i++) {
        registers[i] = 0x00;
    }

    delay_timer = 0x00;
    sound_timer = 0x00;
    reg_I = 0x000;
    PC = 0x0000;
    SP = 0x0000;

    for (int i = 0; i < 16; i++) {
        stack[i] = 0x0000;
    }
}

void emulator::initializeDisplay() {
    dis.initializeWindow();
}

void emulator::run() {
    bool running = true;
    struct instruction_t instr;
    while (running) {
        // load the next instruction to be executed
        instr.left_byte = memory[PC];
        instr.right_byte = memory[PC + 1];

        // store the entire instruction in one var for later convenience
        instr.whole_instr = instr.left_byte;
        instr.whole_instr <<= 8;
        instr.whole_instr |= instr.right_byte;

        // process the instruction
        processInstruction(instr);

    }
}

void emulator::processInstruction(struct instruction_t instr) {
    printf("\nCurrent Instruction: 0x%X_%X\n", instr.left_byte, instr.right_byte);

    // switch with the left most nibble of the instruction
    switch (instr.left_byte & 0xF0) {
        case 0x20:
//            byte subroutineAddress = (byte) (instr.whole_instr & 0x0FFF);
            printf("calling subroutine at 0x%X\n", (unsigned short) (instr.whole_instr & 0x0FFF));

            SP++;
            stack[SP] = PC;
            PC = (unsigned short) (instr.whole_instr & 0x0FFF);
            break;

        case 0x60:
            printf("Setting register V%X to %X\n", instr.left_byte & 0x0F, instr.right_byte);

            // set the register to the appropriate value
            registers[instr.left_byte & 0x0F] = instr.right_byte;

            // increment the PC for the next instruction
            PC += 2;
            break;

        case 0xA0:
            printf("Changing I to %X\n", instr.whole_instr & 0x0FFF);

            // change I to the appropriate value
            reg_I = (unsigned short) (instr.whole_instr & 0x0FFF);

            PC += 2;
            break;

        case 0xD0:
            printf("Gonna display some shit here\n");
            processDisplayInstr(instr);
            dis.update();

            PC += 2;
            break;

        case 0xF0:
            switch (instr.right_byte) {
                case 0x33:
                    printf("Storing %i from register %X in I\n", registers[instr.left_byte & 0x0F], instr.left_byte & 0x0F);

                    // hundreds place
                    memory[reg_I] = (byte) ((registers[instr.left_byte & 0x0F] / 100) % 10);

                    // tens
                    memory[reg_I + 1] = (byte) ((registers[instr.left_byte & 0x0F] / 10) % 10);

                    // ones
                    memory[reg_I + 2] = (byte) ((registers[instr.left_byte & 0x0F] / 1) % 10);

                    PC += 2;
                    break;

                case 0x65:
                    printf("updating registers 0 to %X from memory\n", instr.left_byte & 0x0F);

                    for (int i = 0; i < (instr.left_byte & 0x0F); i++) {
                        registers[i] = memory[reg_I + i];
                    }

                    PC += 2;
                    break;

                default:
                    printf("ERROR: Unknown instruction\n");
                    exit(1);
            }

            break;

        default:
            // unknown or unimplemented instruction - exit
            fprintf(stdout, "ERROR: Unknown instruction\n");
            // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            exit(1);
    }
}

void emulator::processDisplayInstr(instruction_t instr) {
    byte regX = (byte) (instr.left_byte & 0x0F);
    byte regY = (byte) ((instr.right_byte & 0xF0) >> 4);

    printf("Attempting to draw to display at %X, %X\n", registers[regX], registers[regY]);

    // get the number of bytes that the sprite takes
    byte numToDraw = (byte) (instr.right_byte & 0x0F);

    // update the display register with the sprite data
    for (int i = 0; i < numToDraw; ++i) {
        // the bits in each byte represent one line of the sprite
        // get the "line" of the sprite
        byte sprite = memory[reg_I + i];

        for (int j = 0; j < 8; ++j) {

            // if the left most bit is one ...
            if ((byte) (sprite & 0x80) > 0) {
                // check if something will be overwritten
                if (dis.getByteAtIndex(registers[regX] + j, registers[regY] + i)) {
                    registers[0xF] = 1;
                }

                // update the buffer
                dis.setBuffer(registers[regX] + j, registers[regY] + i, 1);
            }
            // shift the sprite one byte to the left to get the next bit during the next iteration
            sprite <<= 1;
        }
    }
}
