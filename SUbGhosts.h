#include "Ghosts.h"
#include <random>
#include <ctime>

// Base Ghost class is already defined in entity.h

// Teleporter Ghost: Teleports to a random allowed area every 10 seconds
class TeleporterGhost : public Ghost {
private:
    float teleportTimer;
    const float TELEPORT_INTERVAL = 10.0f; // Teleport every 10 seconds

public:
    TeleporterGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes),
        teleportTimer(0.0f) {
    }

    void Update(float deltaTime) override {
        Ghost::Update(deltaTime); // Call base class update

        // Update teleport timer
        teleportTimer += deltaTime;
        if (teleportTimer >= TELEPORT_INTERVAL) {
            teleportTimer = 0.0f;
            // Teleport will be handled in updateAutonomous
        }
    }

    void updateAutonomous(Maze& maze) override {
        if (teleportTimer >= TELEPORT_INTERVAL) {
            teleportRandomly(maze);
            teleportTimer = 0.0f;
        }
        else {
            Ghost::updateAutonomous(maze); // Default movement when not teleporting
        }
    }

private:
    void teleportRandomly(Maze& maze) {
        std::random_device rd;
        std::mt19937 gen(rd());

        // Get maze dimensions (you may need to add these getters to your Maze class)
        int rows = maze.getRows(position);
        int cols = maze.getCol(position);
        const float CELL_SIZE = 40.0f; // This should match your maze cell size

        // Try up to 20 random positions until finding a walkable one
        for (int attempts = 0; attempts < 20; attempts++) {
            std::uniform_int_distribution<> rowDist(0, rows - 1);
            std::uniform_int_distribution<> colDist(0, cols - 1);

            int randomRow = rowDist(gen);
            int randomCol = colDist(gen);

            // Convert to screen coordinates (center of cell)
            float x = randomCol * CELL_SIZE + CELL_SIZE / 2;
            float y = randomRow * CELL_SIZE + CELL_SIZE / 2;

            if (maze.isWalkable(sf::Vector2f(x, y))) {
                // Teleport effect - could add visual effects here
                SetPosition(x, y);
                return;
            }
        }

        // If no valid position found after max attempts, just continue normal movement
    }
};

// Chaser Ghost: Always aggressively follows Pacman
class ChaserGhost : public Ghost {
private:
    sf::Vector2f pacmanPosition;
    const float UPDATE_TARGET_INTERVAL = 0.5f; // Update target position every half second
    float updateTimer;

public:
    ChaserGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes),
        updateTimer(0.0f) {
    }

    void Update(float deltaTime) override {
        Ghost::Update(deltaTime);
        updateTimer += deltaTime;
    }

    void updateAutonomous(Maze& maze) override {
        // If we're not at a cell center, continue in current direction
        float CELL_SIZE = 40.0f;
        float centerX = std::round(GetPosition().x / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);
        float centerY = std::round(GetPosition().y / CELL_SIZE) * CELL_SIZE + (CELL_SIZE / 2);

        if (std::abs(GetPosition().x - centerX) > 2.0f || std::abs(GetPosition().y - centerY) > 2.0f) {
            if (!Move(GetCurrentDirection(), maze)) {
                // If blocked, use standard Ghost behavior to find a new path
                Ghost::updateAutonomous(maze);
            }
            return;
        }

        // At an intersection, chase the pacman
        Direction bestDir = findBestDirectionTowardsPacman(maze);
        if (!Move(bestDir, maze)) {
            // If can't move in best direction, use standard behavior
            Ghost::updateAutonomous(maze);
        }
    }

    void setPacmanPosition(const sf::Vector2f& position) {
        pacmanPosition = position;
    }

private:
    Direction findBestDirectionTowardsPacman(Maze& maze) {
        std::vector<Direction> availableDirs = getAvailableDirections(maze);
        if (availableDirs.empty()) return GetCurrentDirection();

        // Calculate distances for each available direction
        Direction bestDir = availableDirs[0];
        float minDistance = std::numeric_limits<float>::max();

        for (Direction dir : availableDirs) {
            sf::Vector2f nextPos = GetPosition();
            float CELL_SIZE = 40.0f;

            switch (dir) {
            case UP:    nextPos.y -= CELL_SIZE; break;
            case DOWN:  nextPos.y += CELL_SIZE; break;
            case LEFT:  nextPos.x -= CELL_SIZE; break;
            case RIGHT: nextPos.x += CELL_SIZE; break;
            }

            float distance = std::sqrt(
                std::pow(nextPos.x - pacmanPosition.x, 2) +
                std::pow(nextPos.y - pacmanPosition.y, 2)
            );

            if (distance < minDistance) {
                minDistance = distance;
                bestDir = dir;
            }
        }

        return bestDir;
    }

    Direction GetCurrentDirection() const {
        // You may need to add this getter to your Ghost class
        return currentDirection;
    }
};

// Ambusher Ghost: Stops at junctions to ambush Pacman
class AmbusherGhost : public Ghost {
private:
    bool isAmbushing;
    float ambushTimer;
    const float AMBUSH_DURATION = 2.0f; // Stop for 2 seconds

public:
    AmbusherGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes),
        isAmbushing(false),
        ambushTimer(0.0f) {
    }

    void Update(float deltaTime) override {
        Ghost::Update(deltaTime);

        if (isAmbushing) {
            ambushTimer += deltaTime;
            if (ambushTimer >= AMBUSH_DURATION) {
                isAmbushing = false;
                ambushTimer = 0.0f;
            }
        }
    }

    void updateAutonomous(Maze& maze) override {
        if (isAmbushing) {
            // Do nothing while ambushing - just wait
            return;
        }

        // Check if we're at a junction (more than 2 possible directions)
        std::vector<Direction> availableDirs = getAvailableDirections(maze);
        if (availableDirs.size() > 2) {
            // We're at a junction - start ambushing
            isAmbushing = true;
            ambushTimer = 0.0f;
            return;
        }

        // Normal movement when not ambushing
        Ghost::updateAutonomous(maze);
    }
};

// RandomGhost: Just follows random paths (already implemented in base Ghost)
class RandomGhost : public Ghost {
public:
    RandomGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes) {
    }

    // Uses default Ghost behavior - purely random movement
};

// HermesGhost: Exceptionally faster than other ghosts
class HermesGhost : public Ghost {
private:
    const float SPEED_BOOST = 1.5f; // 50% faster

public:
    HermesGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed* SPEED_BOOST, scale, frameIndexes) {
    }

    // Uses default Ghost behavior but with increased speed
};

// PhantomGhost: Greater hit radius for catching Pacman
class PhantomGhost : public Ghost {
private:
    const float EXTENDED_RADIUS = 1.5f; // 50% larger collision radius

public:
    PhantomGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes) {
    }

    bool GhostCollision(const sf::Vector2f& pacmanPosition) const override {
        // Create larger bounds for collision detection
        sf::FloatRect pacmanBounds(
            pacmanPosition.x - 20.f * EXTENDED_RADIUS,
            pacmanPosition.y - 20.f * EXTENDED_RADIUS,
            40.f * EXTENDED_RADIUS,
            40.f * EXTENDED_RADIUS
        );

        return getSprite().getGlobalBounds().intersects(pacmanBounds);
    }
};

// TimeStopGhost: Stops Pacman for 2 seconds on collision
class TimeStopGhost : public Ghost {
private:
    bool hasTimeStopActive;
    float timeStopTimer;
    const float TIME_STOP_DURATION = 2.0f;
    const float TIME_STOP_COOLDOWN = 10.0f; // Cooldown for ability
    float cooldownTimer;

public:
    TimeStopGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes),
        hasTimeStopActive(false),
        timeStopTimer(0.0f),
        cooldownTimer(0.0f) {
    }

    void Update(float deltaTime) override {
        Ghost::Update(deltaTime);

        if (hasTimeStopActive) {
            timeStopTimer += deltaTime;
            if (timeStopTimer >= TIME_STOP_DURATION) {
                hasTimeStopActive = false;
                timeStopTimer = 0.0f;
            }
        }

        if (cooldownTimer > 0) {
            cooldownTimer -= deltaTime;
        }
    }

    bool canTimeStopPacman() const {
        return cooldownTimer <= 0.0f;
    }

    void activateTimeStop() {
        if (canTimeStopPacman()) {
            hasTimeStopActive = true;
            timeStopTimer = 0.0f;
            cooldownTimer = TIME_STOP_COOLDOWN;
        }
    }

    bool isTimeStopActive() const {
        return hasTimeStopActive;
    }
};

// RingGhost: Disappears for 3 seconds but can still move and collide with Pacman
class RingGhost : public Ghost {
private:
    bool isVisible;
    float visibilityTimer;
    const float INVISIBLE_DURATION = 3.0f;
    const float VISIBLE_DURATION = 5.0f;

public:
    RingGhost(const std::string& spriteSheetPath,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float speed, float scale,
        const std::map<Direction, int>& frameIndexes)
        : Ghost(spriteSheetPath, frameCount, frameWidth, frameHeight, x, y, speed, scale, frameIndexes),
        isVisible(true),
        visibilityTimer(0.0f) {
    }

    void Update(float deltaTime) override {
        Ghost::Update(deltaTime);

        visibilityTimer += deltaTime;

        if (isVisible && visibilityTimer >= VISIBLE_DURATION) {
            isVisible = false;
            visibilityTimer = 0.0f;
            updateVisibility();
        }
        else if (!isVisible && visibilityTimer >= INVISIBLE_DURATION) {
            isVisible = true;
            visibilityTimer = 0.0f;
            updateVisibility();
        }
    }

    void updateVisibility() {
        sf::Color color = getSprite().getColor();
        color.a = isVisible ? 255 : 0; // Full opacity when visible, transparent when invisible
        setColor(color);
    }

    // Override the Draw method to control visibility
    sf::Sprite getSprite() const override {
        return Ghost::getSprite();
    }

    // RingGhost can still collide even when invisible
    bool GhostCollision(const sf::Vector2f& pacmanPosition) const override {
        return Ghost::GhostCollision(pacmanPosition);
    }
};

// Factory function to create a specific ghost based on type name
Ghost* createSpecialGhost(const std::string& ghostType,
    const std::string& spriteSheetPath,
    float x, float y, float baseSpeed) {
    int frameCount = 4;
    int frameWidth = 50;
    int frameHeight = 50;
    float scale = 1.3f;

    std::map<Direction, int> frameIndexes = {
        {RIGHT, 0}, {UP, 1}, {DOWN, 2}, {LEFT, 3}
    };

    if (ghostType == "TELEPORTER") {
        return new TeleporterGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "CHASER") {
        return new ChaserGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "AMBUSHER") {
        return new AmbusherGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "RANDOMGHOST") {
        return new RandomGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "HERMES") {
        return new HermesGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "PHANTOM") {
        return new PhantomGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "TIMESTOP") {
        return new TimeStopGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else if (ghostType == "RINGGHOST") {
        return new RingGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
    else {
        // Default to random ghost if type not recognized
        return new RandomGhost(spriteSheetPath, frameCount, frameWidth, frameHeight,
            x, y, baseSpeed, scale, frameIndexes);
    }
}