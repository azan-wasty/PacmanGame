#pragma once
#include <map>
#include "Animation.h"
#include "maze.h"
#include "entity.h"

class Pacman : public Entity {
private:
    std::map<Direction, Texture> textures;
    Animation animation;
    Direction currentDirection;
    Vector2f initialPosition;
    float scaleFactor = 0.85f;
    float collisionOffset = 6.f; // Adjust as needed for accurate collision

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
        sprite.setScale(scaleFactor, scaleFactor);
        sprite.setPosition(position);
    }

    void SetDirection(Direction dir) {
        currentDirection = dir;
    }

    void MenuMove() {
        position.x += speed;
        sprite.setPosition(position);
    }

    void Move(Direction dir, Maze& maze) {
        Vector2f tempPosition = position;

        switch (dir) {
        case RIGHT: tempPosition.x += speed; break;
        case LEFT:  tempPosition.x -= speed; break;
        case UP:    tempPosition.y -= speed; break;
        case DOWN:  tempPosition.y += speed; break;
        }

        // Use unscaled, consistent collision center
        FloatRect futureBounds = sprite.getGlobalBounds();
        futureBounds.left = tempPosition.x + collisionOffset;
        futureBounds.top = tempPosition.y + collisionOffset;
        futureBounds.width -= 2 * collisionOffset;
        futureBounds.height -= 2 * collisionOffset;

        Vector2f center = {
            futureBounds.left + futureBounds.width / 2,
            futureBounds.top + futureBounds.height / 2
        };

        // Wrap Pacman horizontally
        float mazeWidth = Maze::getWidth() * Maze::getCellSize() + maze.getOffset().x;
        if (tempPosition.x < maze.getOffset().x) {
            tempPosition.x = mazeWidth - sprite.getGlobalBounds().width - 10;
        }
        else if (tempPosition.x + sprite.getGlobalBounds().width > mazeWidth) {
            tempPosition.x = maze.getOffset().x + 10;
        }

        if (maze.isWalkable(center)) {
            position = tempPosition;
            currentDirection = dir;
            sprite.setPosition(position);
        }
        else {
            currentDirection = dir; // still face in that direction
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

    void SuperScale() {
        sprite.setScale(0.9f, 0.9f);
    }

    void ResetScale() {
        sprite.setScale(scaleFactor, scaleFactor);
    }

    void Update() {
        sprite.setTexture(textures[currentDirection]);
        animation.update(0.075f, currentDirection, sprite);
        sprite.setPosition(position);
    }

    void Reset() {
        position = initialPosition;
        SnapToGrid(); // make sure it's aligned with maze grid
        sprite.setPosition(position);
        animation.reset();
    }
    void setColor(const sf::Color& color) {
        sprite.setColor(color);
    }
    void SnapToGrid() {
        float cs = Maze::getCellSize();
        position.x = round(position.x / cs) * cs;
        position.y = round(position.y / cs) * cs;
    }
};