#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

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
        offset = Vector2f(60.f, 20.f);  // Fixed offset
        srand(static_cast<unsigned>(time(nullptr)));
        wallColor = Color(rand() % 256, rand() % 256, rand() % 256);
        reset();
    }

    void setSuperMode(bool mode) {
        superMode = mode;
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
                if (c == '.')
                    totalFood++;
            }
        }
    }

    void draw(RenderWindow& window) {
        Color drawColor = superMode ? Color::Red : wallColor;

        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                char tile = map[row][col];
                float x = offset.x + col * CELL_SIZE;
                float y = offset.y + row * CELL_SIZE;

                if (tile == '#') {
                    RectangleShape wall(Vector2f(CELL_SIZE, CELL_SIZE));
                    wall.setPosition(x, y);
                    wall.setFillColor(drawColor);
                    wall.setOutlineColor(Color::White);
                    wall.setOutlineThickness(2);
                    window.draw(wall);
                }
                else if (tile == '.') {
                    CircleShape dot(CELL_SIZE / 8);
                    dot.setFillColor(Color::White);
                    dot.setPosition(x + CELL_SIZE / 2.5f, y + CELL_SIZE / 2.5f);
                    window.draw(dot);
                }
                else if (tile == 'o') {
                    CircleShape energizer(CELL_SIZE / 4);
                    energizer.setFillColor(Color::Yellow);
                    energizer.setPosition(x + CELL_SIZE / 3.f, y + CELL_SIZE / 3.f);
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

    bool isWalkable(Vector2f pos) const {
        Vector2i cell = getCell(pos);
        char tile = getTile(cell.y, cell.x);
        return tile != '#' && tile != ' ';
    }

    bool isFood(Vector2f pos) {
        Vector2i cell = getCell(pos);
        if (getTile(cell.y, cell.x) == '.') {
            map[cell.y][cell.x] = ' ';
            totalFood--;
            return true;
        }
        return false;
    }

    bool isSuperFood(Vector2f pos) {
        Vector2i cell = getCell(pos);
        if (getTile(cell.y, cell.x) == 'o') {
            map[cell.y][cell.x] = ' ';
            totalFood--;
            return true;
        }
        return false;
    }

    int getFoodCount() const { return totalFood; }

    Vector2f getOffset() const { return offset; }

    Vector2i getCell(Vector2f pos) const {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);
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
                if (map[row][col] == ghostId)
                    return { col, row };
            }
        }
        return { -1, -1 };
    }

    Vector2i getGhost0() const { return getGhost('0'); }
    Vector2i getGhost1() const { return getGhost('1'); }
    Vector2i getGhost2() const { return getGhost('2'); }
    Vector2i getGhost3() const { return getGhost('3'); }
};
