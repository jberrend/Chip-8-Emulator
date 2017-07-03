//
// Created by jon on 7/3/17.
//

#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "InputHandler.h"

bool InputHandler::isKeyDown(unsigned char key) {
    switch (key) {
        case 0x00:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::X);

        case 0x01:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);

        case 0x02:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);

        case 0x03:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);

        case 0x04:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Q);

        case 0x05:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::W);

        case 0x06:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::E);

        case 0x07:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::A);

        case 0x08:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::S);

        case 0x09:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        case 0x0A:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Z);

        case 0x0B:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::C);

        case 0x0C:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);

        case 0x0D:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::R);

        case 0x0E:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::F);

        case 0x0F:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::V);

        default:
            printf("Unknown key pressed, ignoring\n");
            return false;
    }
}
