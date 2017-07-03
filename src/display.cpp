//
// Created by jon on 6/29/17.
//

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "display.h"
#include <list>

void display::setBuffer(int i, int j, byte value) {
    buffer[i][j] = value;

}

void display::initializeWindow() {
    // pixels are 20x20
    window = new sf::RenderWindow(sf::VideoMode(1280, 640), "Chip8 Emulator");
}

void display::update() {
    // needed to prevent the window from becoming unresponsive
    sf::Event event;
    while (window->pollEvent(event)) {
        // TODO: handle window events here

        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }

    // update and display the next frame
    window->clear(sf::Color::Black);

    // draw everything here
    // get the width and height of each 'pixel' based on the current size of the window
    int pixelWidth = window->getSize().x / 64;
    int pixelHeight = window->getSize().y / 32;

    // list to store all the shapes that will be drawn
    std::list<sf::RectangleShape> list;

    // TODO: Only generate rectangles if the pixel needs to be displayed
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            sf::RectangleShape rect(sf::Vector2f(pixelWidth, pixelHeight));
            rect.setPosition(sf::Vector2f(pixelWidth * i, pixelHeight * j));
            if (buffer[i][j] != 0) {
                rect.setFillColor(sf::Color::White);
            } else {
                rect.setFillColor(sf::Color::Black);
            }

            // add to the list
            list.push_back(rect);
        }
    }

    for (sf::RectangleShape r : list) {
        window->draw(r);
    }

    // finally display the frame
    window->display();
}

byte display::getByteAtIndex(int x, int y) {
    return buffer[x][y];
}

void display::clearBuffer() {
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            buffer[i][j] = 0x00;
        }
    }
}
