#include <iostream>
#include <SFML/Graphics.hpp>
#include "emulator.h"

int main() {
//    // load the rom into a file stream
//    std::ifstream rom("Games/PONG", std::ifstream::in | std::ifstream::binary);
//    rom.seekg(0, rom.end);
//    long size = rom.tellg();
//    rom.seekg(0, rom.beg);
//
//    // buffer to hold the file contents
//    unsigned char buffer[size];
//    // initialize the buffer
//    for (int i = 0; i < size; i++) {
//        buffer[i] = 0;
//    }
//    // read the rom into the buffer
//    rom.read((char *)buffer, size);
//
//    // print it to the screen
//    for (int i = 0; i < size; i++) {
//        printf("0x%X ", buffer[i]);
//    }
//    rom.close();

    emulator emu;
    emu.loadRom("../Games/PONG");
    emu.initializeDisplay();
    emu.run();
    return 0;
}