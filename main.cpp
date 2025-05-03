#include <SFML/Graphics.hpp>
#include "maze.h"
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

void MainGame() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowHeight = 1050;
    int windowWidth = 960;

    RenderWindow window(VideoMode(windowWidth, windowHeight), "Pac-Man");
    window.setFramerateLimit(60);

    Font font;
    font.loadFromFile("ArcadeClassic.ttf");

    Text title("PAC-MAN", font, 80);
    title.setFillColor(Color::Yellow);
    title.setPosition(windowWidth / 2.f - title.getGlobalBounds().width / 2.f, 200);

    vector<string> menuItems = { "Start Game", "Options", "Exit" };
    vector<Text> menuTexts;
    int selectedItem = 0;

    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(Color::White);
        item.setPosition(windowWidth / 2.f - item.getGlobalBounds().width / 2.f, 320 + i * 90);
        menuTexts.push_back(item);
    }

    struct Dot { CircleShape shape; float speed; };
    vector<Dot> dots;
    for (int i = 0; i < 100; ++i) {
        CircleShape dot(2);
        dot.setFillColor(Color::White);
        dot.setPosition(rand() % windowWidth, rand() % windowHeight);
        float speed = 1.f + static_cast<float>(rand() % 100) / 100.f;
        dots.push_back({ dot, speed });
    }

    bool inMenu = true;
    bool gameStarted = false;
    Maze maze;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed) {
                if (inMenu) {
                    if (event.key.code == Keyboard::Up)
                        selectedItem = (selectedItem - 1 + menuItems.size()) % menuItems.size();
                    else if (event.key.code == Keyboard::Down)
                        selectedItem = (selectedItem + 1) % menuItems.size();
                    else if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Return) {
                        if (selectedItem == 0) {
                            inMenu = false;
                            gameStarted = true;
                        }
                        else if (selectedItem == 2) {
                            window.close();
                        }
                    }
                }
            }
        }

        window.clear(Color::Black);

        for (auto& d : dots) {
            Vector2f pos = d.shape.getPosition();
            pos.y += d.speed;
            if (pos.y > windowHeight) pos.y = 0;
            d.shape.setPosition(pos);
            window.draw(d.shape);
        }

        if (inMenu) {
            window.draw(title);
            for (size_t i = 0; i < menuTexts.size(); ++i) {
                menuTexts[i].setFillColor(i == selectedItem ? Color::Yellow : Color::White);
                window.draw(menuTexts[i]);
            }
        }
        else if (gameStarted) {
            maze.draw(window);
        }

        window.display();
    }
}

int main() {
    MainGame();
    return 0;
}
