//
// Created by jon on 6/29/17.
//

#include <chrono>
#include <thread>
#include <iostream>
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

    // seed the RNG
    srand(time(NULL));
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

    loadSprites();
}

void emulator::initializeDisplay() {
    dis.initializeWindow();
}

void emulator::run() {
    bool running = true;
    struct instruction_t instr;

    // start the timer that ensures paces the program's execution and
    // timer registers
    auto instrStartTime = std::chrono::high_resolution_clock::now();
    auto nextTimerTick = instrStartTime + std::chrono::milliseconds(1000/60); // when the timer should tick
    // the time execution of the next instruction should ideally be finished before (1/500 of a second)

    // registers[0xF] = 0x01; // why is this here? <--

    auto endGoalTime = instrStartTime + std::chrono::milliseconds(1000 / 250);

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

        // maybe?
        // dis.clearBuffer();

        // update timer registers // TODO: implement threading or something to update these at a 60Hz interval
        if (nextTimerTick < instrStartTime) {
            if (delay_timer > 0) {
                delay_timer--;
            }
            if (sound_timer > 0) {
                sound_timer--;
            }

            // update the time for the next tick
            nextTimerTick += std::chrono::milliseconds(1000 / 60);
        }

        // sleep until end goal time
        if (std::chrono::high_resolution_clock::now() > endGoalTime) {
            std::cerr << "WARNING: Instruction took longer than expected" << std::endl;
        }
        std::this_thread::sleep_until(endGoalTime);

        instrStartTime = std::chrono::high_resolution_clock::now();
        endGoalTime = instrStartTime + std::chrono::milliseconds(1000 / 500);

    }
}

void emulator::processInstruction(struct instruction_t instr) {
    printf("\nCurrent Instruction: 0x%X_%X\n", instr.left_byte, instr.right_byte);

    // switch with the left most nibble of the instruction
    switch (instr.left_byte & 0xF0) {
        case 0x00:
            switch (instr.right_byte) {
                case 0x00:
                    printf("clearing display");
                    dis.clearBuffer();
                    PC += 2;
                    break;

                case 0xEE:
                    printf("Return from Subroutine to 0x%X", stack[SP]);
                    PC = stack[SP];
                    SP--;
                    PC += 2;
                    break;

                default:
                    printf("ERROR: Unknown instruction\n");
                    exit(1);
            }

            break;

        case 0x10:
            printf("Jumping to %X\n", instr.whole_instr & 0x0FFF);
            PC = (unsigned short) (instr.whole_instr & 0x0FFF);
            break;
        case 0x20:
            printf("calling subroutine at 0x%X\n", (unsigned short) (instr.whole_instr & 0x0FFF));

            SP++;
            stack[SP] = PC;
            PC = (unsigned short) (instr.whole_instr & 0x0FFF);
            break;

        case 0x30:
            printf("Skipping next instr if register %X = %X\n", instr.left_byte & 0x0F, instr.right_byte);

            // if instr is to be skipped, increment the PC an additional time
            if (registers[instr.left_byte & 0x0F] == instr.right_byte) {
                printf("Skipping.\n");
                PC += 2;
            }

            PC += 2;
            break;

        case 0x40:
            printf("Skipping if %X != %X\n", registers[instr.left_byte & 0x0F], instr.right_byte);
            if (registers[instr.left_byte & 0x0F] != instr.right_byte) {
                printf("Skipping\n");
                PC += 2;
            }
            PC += 2;
            break;

        case 0x60:
            printf("Setting register V%X to %X\n", instr.left_byte & 0x0F, instr.right_byte);

            // set the register to the appropriate value
            registers[instr.left_byte & 0x0F] = instr.right_byte;

            // increment the PC for the next instruction
            PC += 2;
            break;

        case 0x70:
            printf("adding 0x%X to register:%X \n", instr.right_byte, instr.left_byte & 0x0F);
            registers[instr.left_byte & 0x0F] += instr.right_byte;

            PC += 2;
            break;

        case 0x80:
            // determine which instruction to execute
            switch (instr.right_byte & 0x0F) {
                case 0x00:
                    printf("Assigning register to another register\n");
                    registers[instr.left_byte & 0x0F] = registers[(instr.right_byte & 0xF0) >> 4];

                    break;
                case 0x02:
                    printf("ANDing registers\n");
                    registers[instr.left_byte & 0x0F] &= registers[(instr.right_byte & 0xF0) >> 4];
                    break;

                case 0x03:
                    printf("XOR registers\n");
                    registers[instr.left_byte & 0x0F] ^= registers[(instr.right_byte & 0xF0) >> 4];
                    break;

                case 0x04: // TODO: Check if this works properly
                    printf("Adding registers together checking for overflow\n");
                    printf("Register %X (%X) + %X (%X)\n", instr.left_byte & 0x0F, registers[instr.left_byte & 0x0F],
                           (instr.right_byte & 0xF0) >> 4, registers[(instr.right_byte & 0xF0) >> 4]);
                    if ((int)registers[instr.left_byte & 0x0F] + (int)registers[(instr.right_byte & 0xF0) >> 4] > 255) {
                        printf("Carry set\n");
                        registers[0xF] = 1;
                    } else {
                        printf("Carry unset\n");
                        registers[0xF] = 0;
                    }

                    registers[instr.left_byte & 0x0F] += registers[(instr.right_byte & 0xF0) >> 4];
                    break;

                case 0x05:
                    printf("Subtracting registers\n");
                    if (registers[instr.left_byte & 0x0F] > registers[(instr.right_byte & 0xF0) >> 4]) {
                        registers[0xF] = 1;
                    } else {
                        registers[0xF] = 0;
                    }

                    registers[instr.left_byte & 0x0F] -= registers[(instr.right_byte & 0xF0) >> 4];

                    break;

                default:
                    printf("ERROR: Unknown instruction\n");
                    exit(1);
            }

            PC += 2;
            break;
        case 0xA0:
            printf("Changing I to %X\n", instr.whole_instr & 0x0FFF);

            // change I to the appropriate value
            reg_I = (unsigned short) (instr.whole_instr & 0x0FFF);

            PC += 2;
            break;

        case 0xC0:
            printf("generating random number... \n");
            registers[instr.left_byte & 0x0F] = (byte) (rand() % 256) & instr.right_byte;

            PC += 2;
            break;

        case 0xD0:
            printf("Gonna display some shit here\n");
            processDisplayInstr(instr);
            dis.update();

            PC += 2;
            break;

        case 0xE0:
            printf("Skipping next instr if key %X is pressed\n", registers[instr.left_byte & 0x0F]);

            // is the key down?
            if (inputHandler.isKeyDown(registers[instr.left_byte & 0x0F])) {
                // skip.
                printf("Skipping.\n");
                PC += 2;
            }

            PC += 2;
            break;
        case 0xF0:
            switch (instr.right_byte) {
                case 0x07:
                    printf("Setting register %X to %X (delay timer)\n", instr.left_byte & 0x0F, delay_timer);
                    registers[instr.left_byte & 0x0F] = delay_timer;

                    PC += 2;
                    break;

                case 0x15:
                    printf("Updating delay timer to 0x%X\n", registers[instr.left_byte & 0x0F]);
                    delay_timer = registers[instr.left_byte & 0x0F];

                    PC += 2;
                    break;

                case 0X18:
                    printf("Setting sound timer\n");
                    sound_timer = registers[instr.left_byte & 0x0F];

                    PC += 2;
                    break;

                case 0x29:
                    printf("Setting I to location of sprite for the digit located in register %X: %X\n", instr.left_byte & 0x0F,
                           registers[instr.left_byte & 0x0F]);

                    processSpriteLoadInstr(instr);

                    PC += 2;
                    break;

                case 0x33:
                    printf("Storing %i from register %X in I\n", registers[instr.left_byte & 0x0F],
                           instr.left_byte & 0x0F);

                    // hundreds place
                    memory[reg_I]     = (byte) ((registers[instr.left_byte & 0x0F] / 100) % 10);

                    // tens
                    memory[reg_I + 1] = (byte) ((registers[instr.left_byte & 0x0F] / 10) % 10);

                    // ones
                    memory[reg_I + 2] = (byte) ((registers[instr.left_byte & 0x0F] / 1) % 10);

                    PC += 2;
                    break;

                case 0x65:
                    printf("updating registers 0 to %X from memory\n", instr.left_byte & 0x0F);

                    // this <= right here took me 3 days of headscratching to find...
                    for (int i = 0; i <= (instr.left_byte & 0x0F); i++) {
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
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            exit(1);
    }
}

void emulator::processDisplayInstr(instruction_t instr) {


    // clear V[F] before drawing to the screen
    registers[0xF] = 0x00;

    byte regX = (byte) (instr.left_byte & 0x0F);
    byte regY = (byte) ((instr.right_byte & 0xF0) >> 4);

    // variable to hold the x and y coord of each "pixel" of the sprite
    int pixelX, pixelY;

    printf("Attempting to draw to display at %X, %X\n", registers[regX], registers[regY]);

    // get the number of bytes that the sprite takes
    byte numToDraw = (byte) (instr.right_byte & 0x0F);

    // update the display register with the sprite data
    for (int i = 0; i < numToDraw; ++i) {
        // the bits in each byte represent one line of the sprite
        // get the "line" of the sprite
        byte sprite = memory[reg_I + i];

        for (int j = 0; j < 8; ++j) {
            // update the pixel coordinates
            pixelX = registers[regX] + j;
            pixelY = registers[regY] + i;

            // adjust the pixel position if it isn't within the displays range (wraparound)
            while(pixelX > 63) {
                pixelX -= 64;
            }
            while(pixelY > 31) {
                pixelY -= 32;
            }

            // if the left most bit is one ...
            if ((byte) (sprite & 0x80) > 0) {
                // check if something will be overwritten
                if (dis.getByteAtIndex(pixelX, pixelY)) {
                    registers[0xF] = 1;
                }

                // update the buffer
                dis.setBuffer(pixelX, pixelY, 1);
            }
            // shift the sprite one byte to the left to get the next bit during the next iteration
            sprite <<= 1;
        }
    }
}

void emulator::processSpriteLoadInstr(instruction_t instr) {
    byte spriteToLoad = (byte) (registers[instr.left_byte & 0x0F]);

    switch (spriteToLoad) {
        case 0x00:
            reg_I = spriteLocations::zero;
            break;
        case 0x01:
            reg_I = spriteLocations::one;
            break;
        case 0x02:
            reg_I = spriteLocations::two;
            break;
        case 0x03:
            reg_I = spriteLocations::three;
            break;
        case 0x04:
            reg_I = spriteLocations::four;
            break;
        case 0x05:
            reg_I = spriteLocations::five;
            break;
        case 0x06:
            reg_I = spriteLocations::six;
            break;
        case 0x07:
            reg_I = spriteLocations::seven;
            break;
        case 0x08:
            reg_I = spriteLocations::eight;
            break;
        case 0x09:
            reg_I = spriteLocations::nine;
            break;
        case 0x0A:
            reg_I = spriteLocations::A;
            break;
        case 0x0B:
            reg_I = spriteLocations::B;
            break;
        case 0x0C:
            reg_I = spriteLocations::C;
            break;
        case 0x0D:
            reg_I = spriteLocations::D;
            break;
        case 0x0E:
            reg_I = spriteLocations::E;
            break;
        case 0x0F:
            reg_I = spriteLocations::F;
            break;

        default:
            printf("ERROR: Sprite out of range");
            exit(1);
    }
}

void emulator::loadSprites() {
    // 0
    memory[0x000] = 0xF0;
    memory[0x001] = 0x90;
    memory[0x002] = 0x90;
    memory[0x003] = 0x90;
    memory[0x004] = 0xF0;
    // 1
    memory[0x005] = 0x20;
    memory[0x006] = 0x60;
    memory[0x007] = 0x20;
    memory[0x008] = 0x20;
    memory[0x009] = 0x70;
    // 2
    memory[0x00A] = 0xF0;
    memory[0x00B] = 0x10;
    memory[0x00C] = 0xF0;
    memory[0x00D] = 0x80;
    memory[0x00E] = 0xF0;
    // 3
    memory[0x00F] = 0xF0;
    memory[0x010] = 0x10;
    memory[0x011] = 0xF0;
    memory[0x012] = 0x10;
    memory[0x013] = 0xF0;
    // 4
    memory[0x014] = 0x90;
    memory[0x015] = 0x90;
    memory[0x016] = 0xF0;
    memory[0x017] = 0x10;
    memory[0x018] = 0x10;
    // 5
    memory[0x019] = 0xF0;
    memory[0x01A] = 0x80;
    memory[0x01B] = 0xF0;
    memory[0x01C] = 0x10;
    memory[0x01D] = 0xF0;
    // 6
    memory[0x01E] = 0xF0;
    memory[0x01F] = 0x80;
    memory[0x020] = 0xF0;
    memory[0x021] = 0x90;
    memory[0x022] = 0xF0;
    // 7
    memory[0x023] = 0xF0;
    memory[0x024] = 0x10;
    memory[0x025] = 0x20;
    memory[0x026] = 0x40;
    memory[0x027] = 0x40;
    // 8
    memory[0x028] = 0xF0;
    memory[0x029] = 0x90;
    memory[0x02A] = 0xF0;
    memory[0x02B] = 0x90;
    memory[0x02C] = 0xF0;
    // 9
    memory[0x02D] = 0xF0;
    memory[0x02E] = 0x90;
    memory[0x02F] = 0xF0;
    memory[0x030] = 0x10;
    memory[0x031] = 0xF0;
    // A
    memory[0x032] = 0xF0;
    memory[0x033] = 0x90;
    memory[0x034] = 0xF0;
    memory[0x035] = 0x90;
    memory[0x036] = 0x90;
    // B
    memory[0x037] = 0xE0;
    memory[0x038] = 0x90;
    memory[0x039] = 0xE0;
    memory[0x03A] = 0x90;
    memory[0x03B] = 0xE0;
    // C
    memory[0x03C] = 0xF0;
    memory[0x03D] = 0x80;
    memory[0x03E] = 0x80;
    memory[0x03F] = 0x80;
    memory[0x040] = 0xF0;
    // D
    memory[0x041] = 0xE0;
    memory[0x042] = 0x90;
    memory[0x043] = 0x90;
    memory[0x044] = 0x90;
    memory[0x045] = 0xE0;
    // E
    memory[0x046] = 0xF0;
    memory[0x047] = 0x80;
    memory[0x048] = 0xF0;
    memory[0x049] = 0x80;
    memory[0x04A] = 0xF0;
    // F
    memory[0x04B] = 0xF0;
    memory[0x04C] = 0x80;
    memory[0x04D] = 0xF0;
    memory[0x04E] = 0x80;
    memory[0x04F] = 0x80;
}
