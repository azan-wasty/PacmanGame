#pragma once
#include <map>
#include "Animation.h"
#include "entity.h"

class Pacman : public Entity {
private:
    std::map<Direction, Texture> textures;
    Animation animation;
    Direction currentDirection;
    Vector2f initialPosition;

public:
    Pacman(const std::map<Direction, std::string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float spd)
        : Entity(x, y, spd), animation(0.075f), currentDirection(RIGHT), initialPosition(x, y)
    {
        for (const auto& pair : spriteSheetPaths) {
            Direction dir = pair.first;
            const string& path = pair.second;

            Texture tex;
            if (!tex.loadFromFile(path)) {
                cerr << "Failed to load Pacman texture: " << path << endl;
                continue;
            }
            textures[dir] = tex;

            for (int i = 0; i < frameCount; ++i) {
                animation.addDirectionalFrame(dir, IntRect(i * frameWidth, 0, frameWidth, frameHeight));
            }
        }

        sprite.setTexture(textures[RIGHT]);
        sprite.setPosition(position);
    }

    void SetDirection(Direction dir) {
        if (currentDirection != dir) {
            currentDirection = dir;
        }
    }

    void Move(Direction dir) {
        currentDirection = dir;
        switch (dir) {
        case RIGHT: position.x += speed; break;
        case UP:    position.y -= speed; break;
        case DOWN:  position.y += speed; break;
        case LEFT:  position.x -= speed; break;
        }
        sprite.setPosition(position);
    }   


    // Getter for Pacman's position
    Vector2f GetPosition() const {
        return position;
    }
    // Returns the animated Pacman sprite for drawing
    const Sprite& getSprite() const {
        return sprite;
    }

    // Returns the current movement direction of Pacman
    Direction GetDirection() const {
        return currentDirection;
    }

    // Stop Pacman's movement
    void Stop() {

    }

    void Update() override {
        sprite.setTexture(textures[currentDirection]);
        animation.update(0.075f, currentDirection, sprite);  // dt can be passed in externally if needed
        sprite.setPosition(position);
    }

    void Reset() override {
        position = initialPosition;
        sprite.setPosition(position);
        animation.reset();
    }
};
