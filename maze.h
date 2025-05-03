#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
using namespace std;
using namespace sf;

class Maze {
private:
    static const int WIDTH = 21;
    static const int HEIGHT = 21;
    static const int CELL_SIZE = 50;

    Vector2f offset;
    std::vector<std::string> map;

public:
    int totalFood = 0;

    Maze() {
        offset = Vector2f((1280 - CELL_SIZE * WIDTH) / 2.f,
            (1200 - CELL_SIZE * HEIGHT - 100) / 2.f);

        const char* mapData[HEIGHT] = {
            " ################### ",
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
            " ################### "
        };

        for (int i = 0; i < HEIGHT; ++i) {
            map.push_back(mapData[i]);
            for (int j = 0; mapData[i][j] != 0; j++) {
                if (mapData[i][j] == '.')
                    totalFood++;
            }
        }
    }

    void reset() {
        totalFood = 0;

        const char* mapData[HEIGHT] = {
            " ################### ",
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
            " ################### "
        };

        map.clear();
        for (int i = 0; i < HEIGHT; ++i) {
            map.push_back(mapData[i]);
            for (int j = 0; mapData[i][j] != 0; j++) {
                if (mapData[i][j] == '.')
                    totalFood++;
            }
        }
    }

    void draw(RenderWindow& window) {
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < map[row].length(); ++col) {
                char tile = map[row][col];
                float x = offset.x + col * CELL_SIZE;
                float y = offset.y + row * CELL_SIZE;

                if (tile == '#') {
                    RectangleShape wall(Vector2f(CELL_SIZE, CELL_SIZE));
                    wall.setPosition(x, y);
                    wall.setFillColor(Color(20, 20, 90));
                    wall.setOutlineColor(Color(0, 0, 255));
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

    bool foodremains() const {
        return totalFood > 0;
    }

    char getTile(int row, int col) const {
        if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH)
            return map[row][col];
        return ' ';
    }

    bool isWall(Vector2f pos) const {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);
        return getTile(row, col) == '#';
    }

    bool isWalkable(Vector2f pos) const {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);
        char tile = getTile(row, col);
        return tile != '#' && tile != ' ';
    }

    bool isFood(Vector2f pos) {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);
        if (getTile(row, col) == '.') {
            map[row][col] = ' ';
            totalFood--;
            return true;
        }
        return false;
    }

    bool isSuperFood(Vector2f pos) {
        int col = static_cast<int>((pos.x - offset.x) / CELL_SIZE);
        int row = static_cast<int>((pos.y - offset.y) / CELL_SIZE);
        if (getTile(row, col) == 'o') {
            map[row][col] = ' ';
            totalFood--;
            return true;
        }
        return false;
    }

    int getFoodCount() const {
        return totalFood;
    }

    Vector2f getOffset() const {
        return offset;
    }

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
            for (int col = 0; col < map[row].length(); ++col) {
                if (map[row][col] == 'P')
                    return { col, row };
            }
        }
        return { -1, -1 };
    }

    Vector2i getGhost(char ghostId) const {
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < map[row].length(); ++col) {
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
