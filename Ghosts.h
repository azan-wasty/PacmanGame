#pragma once
#include <map>
#include "entity.h"
#include "animation.h"

class Ghost : public Entity {
protected:
     map<Direction, Texture> textures;
    Animation animation;
    Direction currentDirection;
    Vector2f initialPosition;

public:
    Ghost(const  map<Direction,  string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float spd)
        : Entity(x, y, spd), animation(0.075f), currentDirection(LEFT), initialPosition(x, y)
    {
        for (const auto& pair : spriteSheetPaths) {
            Direction dir = pair.first;
            const string& path = pair.second;

            Texture tex;
            if (!tex.loadFromFile(path)) {
                cerr << "Failed to load ghost texture: " << path << endl;
                continue;
            }
            textures[dir] = tex;

            for (int i = 0; i < frameCount; ++i) {
                animation.addDirectionalFrame(dir, IntRect(i * frameWidth, 0, frameWidth, frameHeight));
            }
        }

        sprite.setTexture(textures[LEFT]);
        sprite.setPosition(position);
    }
    // Getter for the Ghost's position
    Vector2f GetPosition() const {
        return position;
    }
    // Check if the ghost collides with Pacman
    bool GhostCollision(const Vector2f& pacmanPosition) {
        // Simple collision check: if the positions are the same, it's a collision
        
        if (position == pacmanPosition) {
            return true;  // Collision detected
        }

        return false;  // No collision
    }
    virtual void Move(Direction dir) {
        currentDirection = dir;
        switch (dir) {
        case LEFT:  position.x -= speed; break;
        case RIGHT: position.x += speed; break;
        case UP:    position.y -= speed; break;
        case DOWN:  position.y += speed; break;
        }
        sprite.setPosition(position);
    }

    void Update() {
        sprite.setTexture(textures[currentDirection]);
        animation.update(0.075f, currentDirection, sprite);
        sprite.setPosition(position);
    }

    void Reset() override {
        position = initialPosition;
        sprite.setPosition(position);
        animation.reset();
    }
};
