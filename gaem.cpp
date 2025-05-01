#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

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
    if (!font.loadFromFile("PressStart2P-Regular.ttf")) { // Replace with your font file
        return -1;
    }

    // Title text
    Text title("PAC-MAN", font, 80);
    title.setFillColor(Color::Yellow);
    title.setPosition(windowWidth / 2.f - title.getGlobalBounds().width / 2.f, 60);

    // Menu options
    std::vector<std::string> menuItems = { "Start Game", "Options", "Exit" };
    std::vector<Text> menuTexts;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(Color::White);
        item.setPosition(windowWidth / 2.f - item.getGlobalBounds().width / 2.f, 200 + i * 80);
        menuTexts.push_back(item);
    }

    // Flowing dots setup
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
        float x = static_cast<float>(std::rand() % windowWidth);
        float y = static_cast<float>(std::rand() % windowHeight);
        dot.setPosition(x, y);
        float speed = 1.f + static_cast<float>(std::rand() % 100) / 100.f;
        dots.push_back({ dot, speed });
    }

    // Example Pac-Man-like sprite
    CircleShape pacman(20, 30);
    pacman.setFillColor(Color::Yellow);
    pacman.setPosition(100, windowHeight - 100);

    // Ghost-like sprite
    CircleShape ghost(20);
    ghost.setFillColor(Color::Red);
    ghost.setPosition(200, windowHeight - 100);

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

        window.draw(pacman);
        window.draw(ghost);

        window.display();
    }

    return 0;
}
