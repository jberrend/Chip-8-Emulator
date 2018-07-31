//
// Created by jon on 6/29/17.
//

#ifndef CHIP8EMULATOR_DISPLAY_H
#define CHIP8EMULATOR_DISPLAY_H

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

typedef unsigned char byte;

class display {
private:
    // array of pixels that will be displayed to the window every display update
    byte buffer[64][32];
    sf::RenderWindow *window;

public:
    void setBuffer(int i, int j, byte value);

    byte getByteAtIndex(int x, int y);

    void initializeWindow();

    void update();

    void clearBuffer();
};


#endif //CHIP8EMULATOR_DISPLAY_H
