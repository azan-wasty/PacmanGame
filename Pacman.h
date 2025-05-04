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
    float scaleFactor = 0.77f;  

public:
    Pacman(const map<Direction, string>& spriteSheetPaths,
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
        sprite.setScale(scaleFactor, scaleFactor);  //  Scale down sprite
        sprite.setPosition(position);
    }

    void SetDirection(Direction dir) {
        if (currentDirection != dir) {
            currentDirection = dir;
        }
    }

    void Move(Direction dir, Maze& maze) {
        Vector2f tempPosition = position;

        switch (dir) {
        case RIGHT: tempPosition.x += speed; break;
        case LEFT:  tempPosition.x -= speed; break;
        case UP:    tempPosition.y -= speed; break;
        case DOWN:  tempPosition.y += speed; break;
        }

        FloatRect futureBounds = sprite.getGlobalBounds();
        futureBounds.left = tempPosition.x;
        futureBounds.top = tempPosition.y;

        Vector2f center = {
            futureBounds.left + futureBounds.width / 2,
            futureBounds.top + futureBounds.height / 2
        };

        // Wrap Pacman around horizontally if he exits screen
        float mazeWidth = Maze::getWidth() * Maze::getCellSize() + maze.getOffset().x;
        if (tempPosition.x < maze.getOffset().x) {
            tempPosition.x = mazeWidth - sprite.getGlobalBounds().width; // Wrap to right
        }
        else if (tempPosition.x + sprite.getGlobalBounds().width > mazeWidth) {
            tempPosition.x = maze.getOffset().x; // Wrap to left
        }

        if (maze.isWalkable(center)) {
            position = tempPosition;
            currentDirection = dir;
            sprite.setPosition(position);
        }
        else {
            currentDirection = dir;
        }
    }


    void Stop(Direction dir) {
        // Attempt the movement in the given direction  
        Vector2f tempPosition = position;
        switch (dir) {
        case RIGHT: tempPosition.x += 0; break;
        case LEFT:  tempPosition.x -= 0; break;
        case UP:    tempPosition.y -= 0; break;
        case DOWN:  tempPosition.y += 0; break;
        }
    }

    Vector2f GetPosition() const {
        return position;
    }

    const Sprite& getSprite() const {
        return sprite;
    }

    Direction GetDirection() const {
        return currentDirection;
    }

  

    void Update() override {
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
