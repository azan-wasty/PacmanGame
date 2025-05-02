
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
using namespace std;
using namespace sf;
void Main() {
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
    font.loadFromFile("ArcadeClassic.ttf");

    // Title text
 // Title (moved from y = 60 to y = 200)
    Text title("PAC-MAN", font, 80);
    title.setFillColor(Color::Yellow);
    title.setPosition(windowWidth / 2.f - title.getGlobalBounds().width / 2.f, 200);

    // Menu options (moved from starting y = 200 to y = 320)
    vector< string> menuItems = { "Start Game", "Options", "Exit" };
    vector<Text> menuTexts;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(Color::White);
        item.setPosition(windowWidth / 2.f - item.getGlobalBounds().width / 2.f, 320 + i * 90); // also increased spacing
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


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        // Update dot positions
        for (auto& d : dots) {
            Vector2f pos = d.shape.getPosition();
            pos.y += d.speed;
            if (pos.y > windowHeight)
                pos.y = 0;
            d.shape.setPosition(pos.x, pos.y);
        }

        // Draw everything
        window.clear(Color::Black);

        for (const auto& d : dots)
            window.draw(d.shape);

        window.draw(title);
        for (const auto& item : menuTexts)
            window.draw(item);
		


        window.display();
        getch();
		maze->draw(window);
    }
}
int main() {

	Main();
    return 0;
}