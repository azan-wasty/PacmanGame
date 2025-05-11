#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;
using namespace sf;

class Maze {
private:
    static const int WIDTH = 23;
    static const int HEIGHT = 21;
    static const int CELL_SIZE = 40;

    Vector2f offset;
    vector<string> map;
    Color wallColor;
    bool superMode = false;
    Clock superModeClock;
    const float superDuration = 10.f;
    int totalFood = 0;

    const char* mapData[HEIGHT] = {
        " ###################",
        " #........#........# ",
        " #o##.###.#.###.##o# ",
        " #.................# ",
        " #.##.#.#####.#.##.# ",
        " #....#...#...#....# ",
        " ####.### # ###.#### ",
        "    #.#   0   #.#    ",
        "#####.# ##=## #.#####",
        "     .  #123#  .     ",
        "#####.# ##### #.#####",
        "    #.#       #.#    ",
        " ####.# ##### #.#### ",
        " #........#........# ",
        " #.##.###.#.###.##.# ",
        " #o.#.....P.....#.o# ",
        " ##.#.#.#####.#.#.## ",
        " #....#...#...#....# ",
        " #.######.#.######.# ",
        " #.................# ",
        " ###################"
    };

public:
    Maze() {
        // Default offset position
        offset = Vector2f(60.f, 40.f);

        // Initialize random wall color
        srand(static_cast<unsigned>(time(nullptr)));
        wallColor = Color(rand() % 256, rand() % 256, rand() % 256);

        // Initialize maze
        reset();

        // Debug message to verify offset values
        std::cout << "Maze initialized with offset: (" << offset.x << ", " << offset.y << ")" << std::endl;
    }

    

    void setSuperMode(bool mode) {
        superMode = mode;
        if (mode) {
            superModeClock.restart();
            std::cout << "Super mode activated for " << superDuration << " seconds!" << std::endl;
        }
    }

    bool isSuperModeActive() {
        if (superMode) {
            float remainingTime = superDuration - superModeClock.getElapsedTime().asSeconds();
            if (remainingTime <= 0) {
                superMode = false;
                std::cout << "Super mode expired!" << std::endl;
                return false;
            }
            // Optional: Print remaining time when it's close to expiring
            if (remainingTime < 3.0f && static_cast<int>(remainingTime * 10) % 5 == 0) {
                std::cout << "Super mode ending in " << static_cast<int>(remainingTime + 0.9f) << " seconds!" << std::endl;
            }
        }
        return superMode;
    }

    float getSuperModeTimeRemaining() const {
        if (!superMode) return 0.0f;
        float remainingTime = superDuration - superModeClock.getElapsedTime().asSeconds();
        return (remainingTime > 0) ? remainingTime : 0.0f;
    }

    void reset() {
        map.clear();
        totalFood = 0;
        for (int i = 0; i < HEIGHT; ++i) {
            string row = mapData[i];
            if (row.length() < WIDTH)
                row += string(WIDTH - row.length(), ' ');
            map.push_back(row);
            for (char c : row) {
                if (c == '.' || c == 'o')
                    totalFood++;
            }
        }
    }
    \
    void draw(RenderWindow& window) {
        // Handle super mode color with smooth transition effect
        Color drawColor;
        if (isSuperModeActive()) {
            drawColor = Color::Red;

            // Flash effect when super mode is about to expire (last 3 seconds)
            float remainingTime = getSuperModeTimeRemaining();
            if (remainingTime < 3.0f) {
                // Flash between red and normal color
                if (static_cast<int>(remainingTime * 5) % 2 == 0) {
                    drawColor = Color::Red;
                }
                else {
                    drawColor = wallColor;
                }
            }
        }
        else {
            drawColor = wallColor;
        }

        // Draw timer if in super mode
        if (isSuperModeActive()) {
            float remainingTime = getSuperModeTimeRemaining();
            // Create timer bar at the top of the screen
            RectangleShape timerBar(Vector2f(WIDTH * CELL_SIZE * (remainingTime / superDuration), 10));
            timerBar.setPosition(offset.x, offset.y - 20);
            timerBar.setFillColor(Color::Yellow);
            window.draw(timerBar);
        }

        // Apply rendering adjustment to fix alignment issue (+2, +2 pixels)
        const float renderAdjustX = 2.0f;
        const float renderAdjustY = 2.0f;

        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                char tile = map[row][col];
                // Apply rendering adjustment to visual positions
                float x = offset.x + col * CELL_SIZE + renderAdjustX;
                float y = offset.y + row * CELL_SIZE + renderAdjustY;

                if (tile == '#') {
                    RectangleShape wall(Vector2f(CELL_SIZE, CELL_SIZE));
                    wall.setPosition(x, y+15);
                    wall.setFillColor(drawColor);
                    wall.setOutlineColor(Color::White);
                    wall.setOutlineThickness(2);
                    window.draw(wall);
                }
                else if (tile == '.') {
                    CircleShape dot(CELL_SIZE / 8);
                    dot.setFillColor(Color::White);
                    // Center the dot in the cell
                    dot.setPosition(x + CELL_SIZE / 2.5f, y +15+ CELL_SIZE / 2.5f);
                    window.draw(dot);
                }
                else if (tile == 'o') {
                    CircleShape energizer(CELL_SIZE / 4);
                    energizer.setFillColor(Color::Yellow);
                    // Center the energizer in the cell
                    energizer.setPosition(x + CELL_SIZE / 3.f, y +15+ CELL_SIZE / 3.f);
                    window.draw(energizer);
                }
            }
        }
    }

   

    bool foodremains() const { return totalFood > 0; }

    char getTile(int row, int col) const {
        if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH)
            return map[row][col];
        return ' ';
    }

    bool isWall(Vector2f pos) const {
        Vector2i cell = getCell(pos);
        return getTile(cell.y, cell.x) == '#';
    }

    // Check if position is aligned with grid
    bool isAlignedWithGrid(Vector2f position) {
        float cellX = (position.x - offset.x);
        float cellY = (position.y - offset.y);

        // Check if position is close to center of a cell
        return (fabs(fmod(cellX, CELL_SIZE) - CELL_SIZE / 2.0f) < 2.0f &&
            fabs(fmod(cellY, CELL_SIZE) - CELL_SIZE / 2.0f) < 2.0f);
    }

    // Get the center of the current cell
    Vector2f getCellCenter(Vector2f position) {
        Vector2i cell = getCell(position);
        return Vector2f(
            offset.x + cell.x * CELL_SIZE + CELL_SIZE / 2.0f,
            offset.y + cell.y * CELL_SIZE + CELL_SIZE / 2.0f
        );
    }

    // Improved movement validation
    bool canMove(Vector2f currentPos, Vector2f direction) {
        // Find target position
        Vector2f targetPos = currentPos + direction;

        // Get current and target cells
        Vector2i currentCell = getCell(currentPos);
        Vector2i targetCell = getCell(targetPos);

        // If trying to move to a different cell
        if (currentCell.x != targetCell.x || currentCell.y != targetCell.y) {
            // Check if the target cell is a wall
            if (getTile(targetCell.y, targetCell.x) == '#') {
                return false;
            }

            // When moving diagonally or at corners, ensure path is clear
            if (direction.x != 0 && direction.y != 0) {
                // Check for diagonal movement through walls
                if (getTile(currentCell.y, targetCell.x) == '#' ||
                    getTile(targetCell.y, currentCell.x) == '#') {
                    return false;
                }
            }
        }

        return true;
    }

    bool isWalkable(Vector2f position) {
        int col = static_cast<int>((position.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((position.y - offset.y) / CELL_SIZE);

        if (row < 0 || row >= HEIGHT || col < 0 || col >= WIDTH)
            return false;

        return map[row][col] != '#';
    }

    bool isFood(Vector2f pos) {
        Vector2i cell = getCell(pos);
        Vector2f cellCenter = {
            offset.x + cell.x * CELL_SIZE + CELL_SIZE / 2.f,
            offset.y + cell.y * CELL_SIZE + CELL_SIZE / 2.f
        };

        // Check if the cell contains food
        if (getTile(cell.y, cell.x) == '.') {
            // Calculate distance from player center to cell center
            float distance = sqrt(pow(pos.x - cellCenter.x, 2) + pow(pos.y - cellCenter.y, 2));

            // Use a more appropriate threshold (about 1/3 of cell size)
            if (distance < CELL_SIZE * 0.4f) {
                map[cell.y][cell.x] = ' ';
                totalFood--;
                return true;
            }
        }
        return false;
    }

    bool isSuperFood(Vector2f pos) {
        Vector2i cell = getCell(pos);
        Vector2f cellCenter = {
            offset.x + cell.x * CELL_SIZE + CELL_SIZE / 2.f,
            offset.y + cell.y * CELL_SIZE + CELL_SIZE / 2.f
        };

        // Check if the cell contains super food
        if (getTile(cell.y, cell.x) == 'o') {
            // Calculate distance from player center to cell center
            float distance = sqrt(pow(pos.x - cellCenter.x, 2) + pow(pos.y - cellCenter.y, 2));

            // Use a more appropriate threshold (about 1/3 of cell size)
            if (distance < CELL_SIZE * 0.4f) {
                map[cell.y][cell.x] = ' ';
                totalFood--;
                setSuperMode(true);
                return true;
            }
        }
        return false;
    }

    // Improved collision detection between entities
    bool checkCollision(Vector2f pos1, Vector2f pos2, float threshold = 0.5f) {
        float distance = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
        return distance < CELL_SIZE * threshold;
    }

    int getFoodCount() const { return totalFood; }

    Vector2f getOffset() const { return offset; }

    // Improved entity-to-maze position alignment
    Vector2f alignToGrid(Vector2f pos) {
        Vector2i cell = getCell(pos);
        return Vector2f(
            offset.x + cell.x * CELL_SIZE + CELL_SIZE / 2.0f,
            offset.y + cell.y * CELL_SIZE + CELL_SIZE / 2.0f
        );
    }

    // Convert grid cell coordinates to world position
    Vector2f cellToPosition(int col, int row) {
        return Vector2f(
            offset.x + col * CELL_SIZE + CELL_SIZE / 2.0f,
            offset.y + row * CELL_SIZE + CELL_SIZE / 2.0f
        );
    }

    // Convert world position to perfect grid cell coordinates
    Vector2i getCell(Vector2f pos) const {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);

        // Ensure values are within bounds
        col = std::max(0, std::min(col, WIDTH - 1));
        row = std::max(0, std::min(row, HEIGHT - 1));

        return { col, row };
    }

    static int getCellSize() { return CELL_SIZE; }
    static int getWidth() { return WIDTH; }
    static int getHeight() { return HEIGHT; }

    Vector2i getP() const {
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                if (map[row][col] == 'P')
                    return { col, row };
            }
        }
        return { -1, -1 };
    }

    Vector2i getGhost(char ghostId) const {
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                if (map[row][col] == ghostId) {
                    return { col, row };
                }
            }
        }
        return { -1, -1 };  // Return invalid position if not found
    }

    Vector2i getGhost0() const { return getGhost('0'); }
    Vector2i getGhost1() const { return getGhost('1'); }
    Vector2i getGhost2() const { return getGhost('2'); }
    Vector2i getGhost3() const { return getGhost('3'); }
};