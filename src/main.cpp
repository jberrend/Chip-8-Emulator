#include <iostream>
#include <SFML/Graphics.hpp>
#include "emulator.h"

int main(int argc, char** argv) {

    // get the rom file location from the command line args
    emulator emu;
    if(argc != 2) {
        printf("Must supply ROM path as the only command line arg.\n");
        exit(1);
    }

    // byte num = 1;
    // std::cout << (num / 1) % 10 << std::endl;
    emu.loadRom(argv[1]);
    emu.initializeDisplay();
    emu.run();
    return 0;
}
