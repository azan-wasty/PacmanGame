#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

const int CELL_SIZE = 50;
const int MAP_WIDTH = 23;
const int MAP_HEIGHT = 21;
const int MAP_PIXEL_WIDTH = CELL_SIZE * MAP_WIDTH;   // 1150
const int MAP_PIXEL_HEIGHT = CELL_SIZE * MAP_HEIGHT; // 1050

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 1200;

// Padding to center map vertically (adds space above + below)
const int MAP_OFFSET_X = (WINDOW_WIDTH - MAP_PIXEL_WIDTH) / 2;   // 65
const int MAP_OFFSET_Y = (WINDOW_HEIGHT - MAP_PIXEL_HEIGHT - 100) / 2; // 25 (50 top + 50 bottom approx)

const char* rawMap[MAP_HEIGHT] = {
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

enum GameState { MENU, GAME };

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pac-Man Clone");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("ArcadeClassic.ttf")) return -1;

    GameState gameState = MENU;

    // ---------- MENU SETUP ----------
    Text title("PAC-MAN", font, 80);
    title.setFillColor(Color::Yellow);
    title.setPosition(WINDOW_WIDTH / 2.f - title.getGlobalBounds().width / 2.f, 60);

    std::vector<std::string> menuItems = { "Start Game", "Options", "Exit" };
    std::vector<Text> menuTexts;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(Color::White);
        item.setPosition(WINDOW_WIDTH / 2.f - item.getGlobalBounds().width / 2.f, 200 + i * 80);
        menuTexts.push_back(item);
    }

    struct Dot {
        CircleShape shape;
        float speed;
    };
    std::vector<Dot> dots;
    const int dotCount = 100;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < dotCount; ++i) {
        CircleShape dot(2);
        dot.setFillColor(Color::White);
        float x = static_cast<float>(std::rand() % WINDOW_WIDTH);
        float y = static_cast<float>(std::rand() % WINDOW_HEIGHT);
        dot.setPosition(x, y);
        float speed = 1.f + static_cast<float>(std::rand() % 100) / 100.f;
        dots.push_back({ dot, speed });
    }

    CircleShape pacman(20, 30);
    pacman.setFillColor(Color::Yellow);
    pacman.setPosition(100, WINDOW_HEIGHT - 100);

    CircleShape ghost(20);
    ghost.setFillColor(Color::Red);
    ghost.setPosition(200, WINDOW_HEIGHT - 100);

    // ---------- MAIN LOOP ----------
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (gameState == MENU && event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Enter)
                    gameState = GAME;
                else if (event.key.code == Keyboard::Escape)
                    window.close();
            }
        }

        window.clear(Color::Black);

        if (gameState == MENU) {
            // Animate background dots
            for (auto& d : dots) {
                Vector2f pos = d.shape.getPosition();
                pos.y += d.speed;
                if (pos.y > WINDOW_HEIGHT) pos.y = 0;
                d.shape.setPosition(pos.x, pos.y);
                window.draw(d.shape);
            }

            window.draw(title);
            for (auto& item : menuTexts)
                window.draw(item);
            window.draw(pacman);
            window.draw(ghost);
        }

        else if (gameState == GAME) {
            // --- Draw game map centered with top/bottom padding ---
            for (int row = 0; row < MAP_HEIGHT; ++row) {
                for (int col = 0; col < MAP_WIDTH; ++col) {
                    char tile = rawMap[row][col];
                    float x = MAP_OFFSET_X + col * CELL_SIZE;
                    float y = MAP_OFFSET_Y + row * CELL_SIZE;

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

            // --- Draw stats clearly below the map ---
            Text ui("Score: 0000   Lives: 3", font, 28);
            ui.setFillColor(Color::White);
            ui.setPosition(WINDOW_WIDTH / 2.f - ui.getGlobalBounds().width / 2.f, MAP_OFFSET_Y + MAP_PIXEL_HEIGHT + 20);
            window.draw(ui);
        }

        window.display();
    }

    return 0;
}
