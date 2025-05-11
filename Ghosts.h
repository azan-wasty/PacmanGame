#pragma once
#include "entity.h"
#include "animation.h"
#include "maze.h" 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <cmath>    // For abs(), round(), floor()

class Ghost : public Entity {
private:
    sf::Texture texture;
    Animation animation;
    Direction currentDirection;
    sf::Vector2f initialPosition;
    int frameWidth, frameHeight;
    int currentFrameIndex;
    std::map<int, sf::IntRect> frameMap;
    bool inSuperMode;
    float superModeTimer;
    const float SUPER_MODE_DURATION = 5.0f; // 5 seconds of super mode
    bool isVisible;
    float stuckTimer; // Timer to detect when ghost is stuck
    bool inSpawnBox; // Flag to track if ghost is in spawn box
    float waitTimer; // Timer for waiting before initial movement

    // New behavior variables from second implementation
    float behaviorTimer;     // Timer for ghost behaviors
    bool isScattered;        // For alternating between scatter and random movement
    float scatterTimer;      // For timing scatter/random phases
    const float SCATTER_DURATION = 7.0f;  // Seconds in scatter mode

    // Constants for cell-based movement
    static const int CELL_SIZE = 40;

public:
    // Updated constructor to accept a map of frame indexes for each direction
    Ghost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Entity(x, y, speed),
        animation(0.1f),
        currentDirection(RIGHT), // Start with RIGHT direction to ensure visibility
        initialPosition(x, y),
        frameWidth(frameWidth),
        frameHeight(frameHeight),
        currentFrameIndex(1),
        inSuperMode(false),
        superModeTimer(0.0f),
        isVisible(true),
        stuckTimer(0.0f),
        inSpawnBox(true),
        waitTimer((rand() % 3) + 0.5f), // Random initial wait time between 0.5 and 3.5 seconds
        behaviorTimer(0.0f),
        isScattered(true),
        scatterTimer(0.0f)
    {
        // Seed the random number generator
        static bool seeded = false;
        if (!seeded) {
            srand(static_cast<unsigned int>(time(nullptr)));
            seeded = true;
        }

        if (!texture.loadFromFile(spriteSheetPath)) {
            std::cerr << "Failed to load ghost texture: " << spriteSheetPath << std::endl;
        }

        sprite.setTexture(texture);
        sprite.setScale(scale, scale);

        // Center the sprite on its position
        sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
        sprite.setPosition(position);

        // Add frames per direction based on the passed frameIndexes
        for (const auto& pair : frameIndexes) {
            Direction dir = pair.first;
            int frameIndex = pair.second;

            // Add the frame for the specified direction and frame index
            animation.addDirectionalFrame(
                dir,
                sf::IntRect(frameIndex * frameWidth, 0, frameWidth, frameHeight)
            );
        }

        // Set the initial texture rect
        sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

        // Create frame map for all frames in the spritesheet
        createFrameMap(frameCount);

        // Set initial frame to frame 1
        setFrame(1);
    }

    // Create a map of all frames from the spritesheet
    void createFrameMap(int frameCount) {
        for (int i = 0; i < frameCount; ++i) {
            // For each frame, create an IntRect for its position on the spritesheet
            frameMap[i] = sf::IntRect(
                i * frameWidth,    // X position
                0,                 // Y position (all frames are on the first row)
                frameWidth,        // Width
                frameHeight        // Height
            );
        }
    }

    // Set specific frame for the ghost
    void setFrame(int frameIndex) {
        if (frameMap.find(frameIndex) != frameMap.end()) {
            currentFrameIndex = frameIndex;
            sprite.setTextureRect(frameMap[frameIndex]);
        }
    }

    // Set position directly (needed for menu ghosts)
    void SetPosition(float x, float y) {
        position.x = x;
        position.y = y;
        sprite.setPosition(position);
    }

    bool isStuck(const Vector2f& oldPosition) {
        // Check if the ghost hasn't moved significantly
        return (fabs(position.x - oldPosition.x) < 1.0f &&
            fabs(position.y - oldPosition.y) < 1.0f);
    }

    void tryEscapeSpawnBox(Maze& maze) {
        // Try moving upward to escape the spawn box
        Vector2f tempPosition = position;
        tempPosition.y -= speed * 2.0f; // Try moving up with extra speed

        FloatRect futureBounds = sprite.getGlobalBounds();
        futureBounds.left = tempPosition.x - (futureBounds.width / 2);
        futureBounds.top = tempPosition.y - (futureBounds.height / 2);

        Vector2f center = {
            tempPosition.x,
            tempPosition.y
        };

        if (maze.isWalkable(center)) {
            position = tempPosition;
            sprite.setPosition(position);
            inSpawnBox = false;
        }
        else {
            // If up doesn't work, try other directions with higher speed
            Direction directions[4] = { UP, LEFT, RIGHT, DOWN };
            for (int i = 0; i < 4; i++) {
                tempPosition = position;
                switch (directions[i]) {
                case UP:    tempPosition.y -= speed * 2.0f; break;
                case DOWN:  tempPosition.y += speed * 2.0f; break;
                case LEFT:  tempPosition.x -= speed * 2.0f; break;
                case RIGHT: tempPosition.x += speed * 2.0f; break;
                }

                center = {
                    tempPosition.x,
                    tempPosition.y
                };

                if (maze.isWalkable(center)) {
                    position = tempPosition;
                    sprite.setPosition(position);
                    currentDirection = directions[i];
                    inSpawnBox = false;
                    break;
                }
            }
        }
    }

    // Check if the ghost is at the center of a cell (for decision making at intersections)
    bool isAtCenterOfCell() const {
        // Allow small tolerance for floating point positions
        float tolerance = 2.0f;
        float centerX = std::round(position.x / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);
        float centerY = std::round(position.y / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);

        return std::abs(position.x - centerX) < tolerance &&
            std::abs(position.y - centerY) < tolerance;
    }

    // Get available directions excluding the opposite of current direction
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

            if (isValidDirection(maze, dir)) {
                dirs.push_back(dir);
            }
        }

        // If no directions are valid (i.e., dead-end), go back
        if (dirs.empty()) {
            dirs.push_back(getOpposite(currentDirection));
        }

        return dirs;
    }

    // Check if the ghost can move in the specified direction
    bool isValidDirection(Maze& maze, Direction dir) {
        Vector2f testPos = position;

        // Check where the ghost would be after moving
        switch (dir) {
        case RIGHT: testPos.x += speed; break;
        case LEFT:  testPos.x -= speed; break;
        case UP:    testPos.y -= speed; break;
        case DOWN:  testPos.y += speed; break;
        }

        // Return if this position is walkable
        return maze.isWalkable(testPos);
    }

    // Get the opposite direction
    Direction getOpposite(Direction dir) const {
        switch (dir) {
        case UP:    return DOWN;
        case DOWN:  return UP;
        case LEFT:  return RIGHT;
        case RIGHT: return LEFT;
        default:    return LEFT;
        }
    }

    // New move function integrating cell-based movement from second implementation
    bool Move(Direction dir, Maze& maze) {
        // If ghost is in spawn box and wait timer is still active
        if (inSpawnBox && waitTimer > 0) {
            waitTimer -= 1.0f / 60.0f; // Decrement wait timer
            return false; // Don't move yet
        }

        // If ghost is in spawn box and wait timer is done
        if (inSpawnBox) {
            tryEscapeSpawnBox(maze);
            return true;
        }

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

            // Update the animation frame for the current direction
            animation.update(1.0f / 60.0f, currentDirection, sprite);

            return true;  // Move succeeded
        }

        return false;  // Blocked by wall
    }

    // Autonomous movement integrating behavior patterns from the second implementation
    void updateAutonomous(Maze& maze, const sf::Vector2f& pacmanPosition) {
        float deltaTime = 1.0f / 60.0f;

        // Update behavior timers
        scatterTimer += deltaTime;
        behaviorTimer += deltaTime;

        // Toggle between scatter and chase modes every SCATTER_DURATION seconds
        if (scatterTimer >= SCATTER_DURATION) {
            isScattered = !isScattered;
            scatterTimer = 0.0f;
        }

        // Only change direction at cell centers or when blocked
        if (isAtCenterOfCell() || !Move(currentDirection, maze)) {
            // Get available directions (excludes opposite direction to prevent 180° turns)
            std::vector<Direction> availableDirs = getAvailableDirections(maze);

            if (!availableDirs.empty()) {
                if (isScattered) {
                    // In scatter mode: pick a random valid direction
                    currentDirection = availableDirs[rand() % availableDirs.size()];
                }
                else {
                    // In chase mode: find the direction closest to Pacman
                    Direction bestDir = currentDirection;
                    float minDistance = 999999.0f;

                    for (Direction dir : availableDirs) {
                        // Calculate where ghost would be after moving in this direction
                        Vector2f newPos = position;
                        switch (dir) {
                        case RIGHT: newPos.x += CELL_SIZE; break;
                        case LEFT:  newPos.x -= CELL_SIZE; break;
                        case UP:    newPos.y -= CELL_SIZE; break;
                        case DOWN:  newPos.y += CELL_SIZE; break;
                        }

                        // Calculate distance to Pacman from this new position
                        float dx = newPos.x - pacmanPosition.x;
                        float dy = newPos.y - pacmanPosition.y;
                        float distance = dx * dx + dy * dy; // Square distance (no need for sqrt)

                        if (distance < minDistance) {
                            minDistance = distance;
                            bestDir = dir;
                        }
                    }

                    currentDirection = bestDir;
                }

                // Try the new direction immediately
                Move(currentDirection, maze);
            }
        }
        else {
            // Continue moving in current direction
            Move(currentDirection, maze);
        }

        // Update animation and timers
        Update(deltaTime);
    }

    Direction getRandomDirection() {
        // Get a completely random direction
        Direction directions[4] = { LEFT, RIGHT, UP, DOWN };
        return directions[rand() % 4];
    }

    Direction getRandomDirection(Direction currentDir) {
        // Get a random direction that's not the opposite of current direction
        Direction opposite = getOpposite(currentDir);
        Direction directions[4] = { LEFT, RIGHT, UP, DOWN };
        Direction newDir;

        do {
            newDir = directions[rand() % 4];
        } while (newDir == opposite);

        return newDir;
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

        // Update the animation with the current direction
        animation.update(1.0f / 60.0f, currentDirection, sprite);
    }

    // Call this every frame with deltaTime
    void Update(float deltaTime) {
        // Update animation
        animation.update(deltaTime, currentDirection, sprite);

        // Update wait timer if in spawn box
        if (inSpawnBox && waitTimer > 0) {
            waitTimer -= deltaTime;
        }

        // Handle super mode timer if active
        if (inSuperMode) {
            superModeTimer -= deltaTime;
            if (superModeTimer <= 0) {
                inSuperMode = false;
                setFrame(1); // Reset to normal frame
            }
        }
    }

    void Reset() {
        position = initialPosition;
        sprite.setPosition(position);
        animation.reset();
        setFrame(1); // Reset to normal frame
        inSuperMode = false;
        superModeTimer = 0.0f;
        inSpawnBox = true;
        waitTimer = (rand() % 3) + 0.5f; // Random wait time between 0.5 and 3.5 seconds

        // Reset behavior variables
        isScattered = true;
        scatterTimer = 0.0f;
        behaviorTimer = 0.0f;
    }

    sf::Vector2f GetPosition() const {
        return position;
    }

    sf::Sprite getSprite() const {
        return sprite;
    }

    bool GhostCollision(const sf::Vector2f& pacmanPosition) const {
        return sprite.getGlobalBounds().intersects(sf::FloatRect(pacmanPosition, sf::Vector2f(40, 40)));
    }

    // SuperMode function that handles collision with Pacman
    void SuperMode(const sf::Vector2f& pacmanPosition) {
        if (GhostCollision(pacmanPosition)) {
            // Change to frame 7 if collision occurs
            setFrame(7);
            // Return ghost to initial position
            position = initialPosition;
            sprite.setPosition(position);
            // Enter super mode
            inSuperMode = true;
            superModeTimer = SUPER_MODE_DURATION;
            // Reset spawn box state
            inSpawnBox = true;
            waitTimer = (rand() % 3) + 0.5f; // Random wait time
        }
    }

    // Static method to create ghosts based on their ID from the maze
    static Ghost* CreateGhostForPosition(Maze& maze, char ghostId, float speed, float scale) {
        // Path to the ghost spritesheet
        std::string spritePath = "ghost16.png";

        // Define frame dimensions
        int frameWidth = 32;
        int frameHeight = 32;

        // Total number of frames in the spritesheet
        int frameCount = 16;

        // Get the maze offset (starting position)
        Vector2f mazeOffset = maze.getOffset();

        // Hard-coded spawn box center position - we'll offset ghosts from this point
        Vector2f spawnBoxCenter;

        // The spawn box center is approximately the middle of where '0', '1', '2', '3' are placed
        // Based on the cell-based system
        int spawnBoxCenterRow = 10;
        int spawnBoxCenterCol = 11;

        // Convert to world coordinates
        spawnBoxCenter = maze.cellToPosition(spawnBoxCenterCol, spawnBoxCenterRow);

        // Position offset for each ghost
        Vector2f ghostOffset;
        int cellSize = Maze::getCellSize();

        // Offset each ghost based on their ID
        switch (ghostId) {
        case '0': // Red ghost - top left
            ghostOffset = Vector2f(-cellSize / 2, -cellSize / 2);
            break;
        case '1': // Pink ghost - top right
            ghostOffset = Vector2f(cellSize / 2, -cellSize / 2);
            break;
        case '2': // Cyan ghost - bottom left
            ghostOffset = Vector2f(-cellSize / 2, cellSize / 2);
            break;
        case '3': // Orange ghost - bottom right
            ghostOffset = Vector2f(cellSize / 2, cellSize / 2);
            break;
        default:
            ghostOffset = Vector2f(0, 0);
        }

        // Calculate final position
        Vector2f finalPosition = spawnBoxCenter + ghostOffset;

        std::cout << "Creating ghost " << ghostId << " at position ("
            << finalPosition.x << ", " << finalPosition.y << ")" << std::endl;

        // Select frame color based on ghost ID
        int baseFrame;
        switch (ghostId) {
        case '0': baseFrame = 1; break;
        case '1': baseFrame = 2; break;
        case '2': baseFrame = 3; break;
        case '3': baseFrame = 4; break;
        default:  baseFrame = 1; break;
        }

        // Create frame indexes for each direction
        std::map<Direction, int> frameIndexes = {
            {LEFT, baseFrame},
            {RIGHT, baseFrame},
            {UP, baseFrame},
            {DOWN, baseFrame}
        };

        // Create and return a new Ghost instance
        return new Ghost(spritePath, frameCount, frameWidth, frameHeight,
            finalPosition.x, finalPosition.y, speed, scale, frameIndexes);
    }
};