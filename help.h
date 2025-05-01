#pragma once

#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <iostream>
using namespace std;
using namespace sf;

    // -------- Direction Enum -------- //
bool isCursorKeyPressed(int& whichKey) {
    char key;
    key = GetAsyncKeyState(37);
    if (key == 1) { whichKey = 1; return true; }
    key = GetAsyncKeyState(38);
    if (key == 1) { whichKey = 3; return true; }
    key = GetAsyncKeyState(39);
    if (key == 1) { whichKey = 2; return true; }
    key = GetAsyncKeyState(40);
    if (key == 1) { whichKey = 4; return true; }
    key = GetAsyncKeyState('A');
    if (key == 1)
    {
        whichKey = 6;   // 6 to Swap the the candies left A key is pressed
        return true;
    }
    key = GetAsyncKeyState('W');
    if (key == 1)
    {
        whichKey = 7;   // 7 to Swap the the candies up W key is pressed
        return true;
    }
    key = GetAsyncKeyState('D');
    if (key == 1)
    {
        whichKey = 8;   // 8 to Swap the the candies right D key is pressed
        return true;
    }
    key = GetAsyncKeyState('S');
    if (key == 1)
    {
        whichKey = 9;   // 9 to Swap the the candies Down s key is pressed
        return true;
    }
    return false;
    return false;
}

    // -------- Check Collision (Tile Based) -------- //
    inline bool isColliding(int x1, int y1, int x2, int y2) {
        return (x1 == x2 && y1 == y2);
    }

    // -------- Random Valid Position (for raw 2D char arrays) -------- //
    inline sf::Vector2i getRandomPosition(int rows, int cols, char** maze, char walkable = '.') {
        int x, y;
        do {
            x = rand() % cols;
            y = rand() % rows;
        } while (maze[y][x] != walkable);
        return sf::Vector2i(x, y);
    }

    // -------- Create SFML Text Object -------- //
    inline sf::Text createText(const std::string& str, const sf::Font& font, int size,
        sf::Color color, float x, float y) {
        sf::Text text;
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(x, y);
        return text;
    }

    // -------- Convert Integer to String -------- //
    inline std::string intToString(int value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // -------- Check If Power Mode Has Expired -------- //
    inline bool isPowerModeExpired(sf::Clock& clock, float durationSeconds) {
        return clock.getElapsedTime().asSeconds() >= durationSeconds;
    }

    // -------- Clamp a Value Within Range -------- //
    inline int clamp(int value, int min, int max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }


