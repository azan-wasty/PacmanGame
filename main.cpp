#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include "maze.h"

using namespace std;
using namespace sf;

enum GameState { MENU, GAME, EXIT };

int main() {
    // Get screen size
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Desired window size
    int windowHeight = screenHeight - 50;
    int windowWidth = screenWidth / 2;

    // Create window
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Pac-Man");
    window.setFramerateLimit(60);

    // Load font
    Font font;
    if (!font.loadFromFile("ArcadeClassic.ttf")) {
        return -1;
    }

    // Title text
    Text title("PAC-MAN", font, 80);
    title.setFillColor(Color::Yellow);
    title.setPosition(windowWidth / 2.f - title.getGlobalBounds().width / 2.f, 200);

    // Menu options
    vector<string> menuItems = { "Start Game", "Options", "Exit" };
    vector<Text> menuTexts;
    int selectedItem = 0;

    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(i == selectedItem ? Color::Red : Color::White);
        item.setPosition(windowWidth / 2.f - item.getGlobalBounds().width / 2.f, 320 + i * 90);
        menuTexts.push_back(item);
    }

    // Flowing dots setup
    struct Dot {
        CircleShape shape;
        float speed;
    };
    vector<Dot> dots;
    const int dotCount = 100;
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < dotCount; ++i) {
        CircleShape dot(2);
        dot.setFillColor(Color::White);
        float x = static_cast<float>(rand() % windowWidth);
        float y = static_cast<float>(rand() % windowHeight);
        dot.setPosition(x, y);
        float speed = 1.f + static_cast<float>(rand() % 100) / 100.f;
        dots.push_back({ dot, speed });
    }

    GameState state = MENU;
    Maze maze;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (state == MENU) {
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up) {
                        selectedItem = (selectedItem - 1 + menuItems.size()) % menuItems.size();
                    }
                    else if (event.key.code == Keyboard::Down) {
                        selectedItem = (selectedItem + 1) % menuItems.size();
                    }
                    else if (event.key.code == Keyboard::Enter) {
                        if (menuItems[selectedItem] == "Start Game") {
                            state = GAME;
                        }
                        else if (menuItems[selectedItem] == "Exit") {
                            window.close();
                        }
                        // You can add "Options" functionality later
                    }

                    // Update menu colors
                    for (size_t i = 0; i < menuTexts.size(); ++i)
                        menuTexts[i].setFillColor(i == selectedItem ? Color::Red : Color::White);
                }
            }
        }

        // Update flowing dots
        for (auto& d : dots) {
            Vector2f pos = d.shape.getPosition();
            pos.y += d.speed;
            if (pos.y > windowHeight)
                pos.y = 0;
            d.shape.setPosition(pos.x, pos.y);
        }

        window.clear(Color::Black);

        if (state == MENU) {
            for (const auto& d : dots)
                window.draw(d.shape);

            window.draw(title);
            for (const auto& item : menuTexts)
                window.draw(item);
        }
        else if (state == GAME) {
            maze.draw(window);
        }

        window.display();
    }

    return 0;
}
