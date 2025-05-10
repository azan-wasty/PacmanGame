#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include "animation.h"
#include "Ghosts.h"

class Teleporter : public Ghost {
public:
    Teleporter(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Random : public Ghost {
public:
    Random(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Chaser : public Ghost {
public:
    Chaser(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Ambusher : public Ghost {
public:
    Ambusher(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Hermes : public Ghost {
public:
    Hermes(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Phantom : public Ghost {
public:
    Phantom(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class TimeStop : public Ghost {
public:
    TimeStop(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class RingGhost : public Ghost {
public:
    RingGhost(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight, float x, float y, float spd,float sc)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd,sc)
    {
        sprite.setPosition(x, y);
    }

    void update(Direction dir) {
        animation.updatetele(dir, sprite);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};
