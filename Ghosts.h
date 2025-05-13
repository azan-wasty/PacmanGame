#pragma once

#include "entity.h"
#include "animation.h"
#include "maze.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

class Ghost : public Entity {
protected:
    sf::Texture texture;
    Animation animation;
    Direction currentDirection;
    sf::Vector2f initialPosition;
    int frameWidth, frameHeight;
    float behaviorTimer;     // Timer for ghost behaviors
    bool isScattered;        // For alternating between scatter and random movement
    float scatterTimer;      // For timing scatter/random phases
    const float SCATTER_DURATION = 7.0f;  // Seconds in scatter mode
    sf::Color originalColor; // Store the original color for restoration after super mode

    // Constants for cell-based movement
    static const int CELL_SIZE = 40;

public:
    Ghost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Entity(x, y, speed),
        animation(0.1f),
        currentDirection(LEFT),
        initialPosition(x, y),
        frameWidth(frameWidth),
        frameHeight(frameHeight),
        behaviorTimer(0.0f),
        isScattered(true),
        scatterTimer(0.0f)
    {
        if (!texture.loadFromFile(spriteSheetPath)) {
            std::cerr << "Failed to load ghost texture: " << spriteSheetPath << std::endl;
        }

        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        sprite.setPosition(position);
        originalColor = sprite.getColor(); // Store the original color

        for (int i = 0; i < frameCount; ++i) {
            animation.addFrame(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight)); // horizontal layout
        }

        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    virtual ~Ghost() = default;

    virtual bool Move(Direction dir, Maze& maze) {
        Vector2f tempPosition = position;
        float moveDist = speed;

        switch (dir) {
        case RIGHT: tempPosition.x += moveDist; break;
        case LEFT:  tempPosition.x -= moveDist; break;
        case UP:    tempPosition.y -= moveDist; break;
        case DOWN:  tempPosition.y += moveDist; break;
        }

        if (isValidDirection(maze, dir)) {
            position = tempPosition;
            currentDirection = dir;
            sprite.setPosition(position);
            return true;  // Move succeeded
        }

        return false;  // Blocked by wall
    }

    void menMove(Direction dir) {
        currentDirection = dir;

        switch (dir) {
        case LEFT:  position.x -= speed; break;
        case RIGHT: position.x += speed; break;
        case UP:    position.y -= speed; break;
        case DOWN:  position.y += speed; break;
        }

        sprite.setPosition(position);
        Update(1.0f / 60.0f);
    }

    virtual void Update(float deltaTime) {
        animation.updateGhost(currentDirection, sprite);
    }

    virtual void Reset() {
        position = initialPosition;
        sprite.setPosition(position);
        currentDirection = LEFT;
        isScattered = true;
        scatterTimer = 0.0f;
        behaviorTimer = 0.0f;
        animation.reset();
        sprite.setColor(originalColor); // Reset to original color
    }

    sf::Vector2f GetPosition() const {
        return position;
    }

    void SetPosition(float x, float y) {
        position.x = x;
        position.y = y;
        sprite.setPosition(position);
    }

    virtual sf::Sprite getSprite() const {
        return sprite;
    }

    virtual bool GhostCollision(const sf::Vector2f& pacmanPosition) const {
        sf::FloatRect pacmanBounds(
            pacmanPosition.x - 20.f, // center the 40x40 box around pacmanPosition
            pacmanPosition.y - 20.f,
            40.f,
            40.f
        );

        return sprite.getGlobalBounds().intersects(pacmanBounds);
    }

    virtual void updateAutonomous(Maze& maze) {
        float deltaTime = 1.0f / 60.0f;

        // Try to move in current direction
        if (!Move(currentDirection, maze)) {
            // If blocked, pick a new valid direction (excluding opposite)
            std::vector<Direction> possibleDirs;

            for (int d = 0; d < 4; ++d) {
                Direction dir = static_cast<Direction>(d);
                if (dir == getOpposite(currentDirection)) continue;

                if (isValidDirection(maze, dir)) {
                    possibleDirs.push_back(dir);
                }
            }

            if (!possibleDirs.empty()) {
                currentDirection = possibleDirs[std::rand() % possibleDirs.size()];
                Move(currentDirection, maze);  // Try the new direction immediately
            }
        }

        Update(deltaTime);
    }

    Direction GetCurrentDirection() const {
        return currentDirection;
    }

    std::vector<Direction> getAvailableDirections(Maze& maze) {
        std::vector<Direction> dirs;
        Vector2f testPos;

        // Try each direction, excluding the opposite of current direction
        for (int d = 0; d < 4; ++d) {
            Direction dir = static_cast<Direction>(d);

            // Skip the opposite direction (no backtracking)
            if (dir == getOpposite(currentDirection)) {
                continue;
            }

            testPos = position;

            // Check the position for each direction
            switch (dir) {
            case UP:    testPos.y -= CELL_SIZE; break;
            case DOWN:  testPos.y += CELL_SIZE; break;
            case LEFT:  testPos.x -= CELL_SIZE; break;
            case RIGHT: testPos.x += CELL_SIZE; break;
            }

            // If the new position is walkable, add this direction to the list
            if (maze.isWalkable(testPos)) {
                dirs.push_back(dir);
            }
        }

        // If no directions are valid (i.e., dead-end), go back
        if (dirs.empty()) {
            dirs.push_back(getOpposite(currentDirection));
        }

        return dirs;
    }

    Direction getOpposite(Direction dir) const {
        switch (dir) {
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
        default: return LEFT;
        }
    }

    bool isAwayFromCenterEnough() const {
        float minDistance = 25.0f;  // Ghost must be at least this far from cell center

        float centerX = std::round(position.x / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);
        float centerY = std::round(position.y / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);

        float dx = std::abs(position.x - centerX);
        float dy = std::abs(position.y - centerY);

        return dx > minDistance || dy > minDistance;
    }

    bool isValidDirection(Maze& maze, Direction dir) {
        Vector2f testPos = position;

        // Round to center of current cell
        testPos.x = std::floor(testPos.x / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2;
        testPos.y = std::floor(testPos.y / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2;

        // Move one full cell in the given direction
        switch (dir) {
        case RIGHT: testPos.x += CELL_SIZE; break;
        case LEFT:  testPos.x -= CELL_SIZE; break;
        case UP:    testPos.y -= CELL_SIZE; break;
        case DOWN:  testPos.y += CELL_SIZE; break;
        }

        return maze.isWalkable(testPos);
    }

    void SuperSpeed() {
        speed += 1.f;  // Increase speed   
    }

    void ResetSpeed() {
        speed -= 1.f;  // Reset speed
    }

    // Set the color of the ghost for super mode
    void setColor(const sf::Color& color) {
        sprite.setColor(color);
    }

    // Get the original color of the ghost
    sf::Color getOriginalColor() const {
        return originalColor;
    }
};

