#pragma once
#include "Ghosts.h" // assuming Ghost inherits Entity
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

class Teleporter : public Ghost {
private:
      Clock teleportClock;
    float teleportInterval;
    float tileSize;

public:
    Teleporter(const   map<Direction,   string>& spriteSheetPaths,
        int frameCount, int frameWidth, int frameHeight,
        float x, float y, float spd, float tileSize)
        : Ghost(spriteSheetPaths, frameCount, frameWidth, frameHeight, x, y, spd), tileSize(tileSize)
    {
        teleportClock.restart();
        teleportInterval = static_cast<float>(25 + rand() % 6);
    }

    void Update(float dt, const   Vector2f& pacmanPos, Direction pacmanDir,
        const   vector<  vector<int>>& mapGrid)
    {
        animation.update(dt, currentDirection, sprite);

        if (teleportClock.getElapsedTime().asSeconds() >= teleportInterval) {
            TeleportNearPacman(pacmanPos, pacmanDir, mapGrid);
            teleportClock.restart();
            teleportInterval = static_cast<float>(25 + rand() % 6);
        }
    }

private:
    void TeleportNearPacman(const   Vector2f& pacmanPos, Direction pacmanDir,
        const   vector<  vector<int>>& mapGrid)
    {
          vector<  Vector2f> candidates;

          Vector2f ahead = pacmanPos;
          Vector2f behind = pacmanPos;

        switch (pacmanDir) {
        case RIGHT:
            ahead.x += tileSize * 2;
            behind.x -= tileSize * 2;
            break;
        case LEFT:
            ahead.x -= tileSize * 2;
            behind.x += tileSize * 2;
            break;
        case UP:
            ahead.y -= tileSize * 2;
            behind.y += tileSize * 2;
            break;
        case DOWN:
            ahead.y += tileSize * 2;
            behind.y -= tileSize * 2;
            break;
        }

        if (IsWalkable(ahead, mapGrid)) candidates.push_back(ahead);
        if (IsWalkable(behind, mapGrid)) candidates.push_back(behind);

        if (!candidates.empty()) {
            int choice = rand() % candidates.size();
            SetPosition(candidates[choice].x, candidates[choice].y);
        }
    }

    bool IsWalkable(const   Vector2f& pos, const   vector<  vector<int>>& mapGrid) {
        int col = static_cast<int>(pos.x / tileSize);
        int row = static_cast<int>(pos.y / tileSize);

        if (row >= 0 && row < mapGrid.size() &&
            col >= 0 && col < mapGrid[0].size()) {
            return mapGrid[row][col] == 0; // 0 = walkable
        }
        return false;
    }
};
#pragma once
