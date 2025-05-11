#include <SFML/Graphics.hpp>
#include "maze.h"
#include "pacman.h"
#include "Ghosts.h"
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>

using namespace std;
using namespace sf;

const int windowWidth = 960;
const int windowHeight = 1050;

vector<string> ghostNames = {
    "TELEPORTER", "RANDOMGHOST", "CHASER", "AMBUSHER",
    "HERMES", "PHANTOM", "TIMESTOP", "RINGGHOST"
};

struct Dot {
    CircleShape shape;
    float speed;
};

void generateBackgroundDots(vector<Dot>& dots) {
    for (int i = 0; i < 100; ++i) {
        CircleShape dot(2);
        dot.setFillColor(Color::White);
        dot.setPosition(rand() % windowWidth, rand() % windowHeight);
        float speed = 1.f + static_cast<float>(rand() % 100) / 100.f;
        dots.push_back({ dot, speed });
    }
}

vector<Ghost*> createMenuGhosts() {
    random_device rd;
    mt19937 gen(rd());
    shuffle(ghostNames.begin(), ghostNames.end(), gen);
    vector<Ghost*> ghosts;

    // Ensure that ghostNames contains at least 4 elements.
    if (ghostNames.size() < 4) {
        cerr << "Not enough ghost names to create menu ghosts!" << endl;
        return ghosts;  // Return empty vector if there aren't enough names.
    }

    for (int i = 0; i < 4; ++i) {
        string ghostName = ghostNames[i];
        string spriteSheetPath = "sprites/" + ghostName + ".png";

        // Create frame indexes for all four directions - using the same frame for all directions
        map<Direction, int> frameIndexes = {
            {RIGHT, 0}, {LEFT, 0}, {UP, 0}, {DOWN, 0}
        };

        float x = -120.0f * (i + 1);
        float y = 700.f;

        Ghost* g = new Ghost(spriteSheetPath, 4, 50, 50, x, y, 2.0f, 5.0f, frameIndexes);
        ghosts.push_back(g);
    }

    return ghosts;
}

void updateDots(vector<Dot>& dots) {
    for (auto& d : dots) {
        Vector2f pos = d.shape.getPosition();
        pos.y += d.speed;
        if (pos.y > windowHeight) pos.y = 0;
        d.shape.setPosition(pos);
    }
}

// Function to create ghosts with supermode capability
vector<Ghost*> spawnGameGhosts(RenderWindow& window, const Maze& maze) {
    vector<Ghost*> gameGhosts;
    vector<string> ghostNamesCopy = ghostNames;
    random_device rd;
    mt19937 gen(rd());
    shuffle(ghostNamesCopy.begin(), ghostNamesCopy.end(), gen);

    for (int i = 0; i < 4; ++i) {
        string ghostName = ghostNamesCopy[i];
        string spriteSheetPath = "sprites/" + ghostName + ".png";

        // Create frame indexes for all four directions
        map<Direction, int> frameIndexes = {
            {RIGHT, 0}, {LEFT, 0}, {UP, 0}, {DOWN, 0}
        };

        Vector2i ghostPos = maze.getGhost(i + '0');

        if (ghostPos.x == -1 || ghostPos.y == -1) continue;

        static const int TILE_SIZE = 40;
        float x = ghostPos.x * TILE_SIZE;
        float y = ghostPos.y * TILE_SIZE;

        // Create ghost with frame indexes for normal movement
        Ghost* g = new Ghost(spriteSheetPath, 16, 50, 50, x + 20, y + 20, 2.0f, 1.0f, frameIndexes);

        // Initialize the frame map for supermode functionality
        g->createFrameMap(16);

        // Set initial frame
        g->setFrame(1);

        gameGhosts.push_back(g);
    }

    return gameGhosts;
}

void drawMenu(RenderWindow& window, Text& title, vector<Text>& menuTexts, int selectedItem,
    vector<Ghost*>& menuGhosts, vector<Dot>& dots, bool inMenu) {

    for (auto& d : dots)
        window.draw(d.shape);

    window.draw(title);

    for (size_t i = 0; i < menuTexts.size(); ++i) {
        menuTexts[i].setFillColor(i == selectedItem ? Color::Yellow : Color::White);
        window.draw(menuTexts[i]);
    }

    if (inMenu) {
        for (int i = 0; i < min(4, (int)menuGhosts.size()); ++i) {
            Ghost* g = menuGhosts[i];

            // Move the ghost in the RIGHT direction
            g->menMove(RIGHT);

            if (g->GetPosition().x > windowWidth) {
                // Reset position when ghost goes off screen
                float newX = -190.f;
                g->SetPosition(newX, 700.f);
            }

            window.draw(g->getSprite());
        }
    }
}

// Ghost AI function to determine movement direction
Direction getGhostDirection(Ghost* ghost, const Vector2f& pacmanPos, const Maze& maze, bool isPacmanSuperMode) {
    // Get current ghost position
    Vector2f ghostPos = ghost->GetPosition();

    // If Pacman is in super mode, try to move away from Pacman
    if (isPacmanSuperMode) {
        // Calculate direction vector from Pacman to ghost
        Vector2f dirVec = ghostPos - pacmanPos;

        // Determine which direction to move based on the vector
        if (abs(dirVec.x) > abs(dirVec.y)) {
            // Move horizontally
            return (dirVec.x > 0) ? RIGHT : LEFT;
        }
        else {
            // Move vertically
            return (dirVec.y > 0) ? DOWN : UP;
        }
    }
    // Otherwise, random movement with a slight bias toward Pacman
    else {
        // 70% chance of moving toward Pacman, 30% chance of random movement
        if (rand() % 100 < 70) {
            // Calculate direction vector from ghost to Pacman
            Vector2f dirVec = pacmanPos - ghostPos;

            // Determine which direction to move based on the vector
            if (abs(dirVec.x) > abs(dirVec.y)) {
                // Move horizontally
                return (dirVec.x > 0) ? RIGHT : LEFT;
            }
            else {
                // Move vertically
                return (dirVec.y > 0) ? DOWN : UP;
            }
        }
        else {
            // Random direction
            Direction directions[4] = { LEFT, RIGHT, UP, DOWN };
            return directions[rand() % 4];
        }
    }
}

void MainGame() {
    srand(static_cast<unsigned int>(time(nullptr)));

    RenderWindow window(VideoMode(windowWidth, windowHeight), "Pac-Man");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("ArcadeClassic.ttf")) {
        cerr << "Error: Could not load font ArcadeClassic.ttf" << endl;
        return;
    }

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

    vector<Dot> dots;
    generateBackgroundDots(dots);

    Maze maze;
    Vector2i pacmanCell = maze.getP();
    Vector2f offset = maze.getOffset();
    float cellSize = Maze::getCellSize();
    Vector2f pacmanStartPos(pacmanCell.x * cellSize + offset.x, pacmanCell.y * cellSize + offset.y);

    map<Direction, string> pacPaths = {
        { UP, "sprites/PACMANUP.PNG" },
        { DOWN, "sprites/PACMANDOWN.png" },
        { LEFT, "sprites/PACMANLEFT.png" },
        { RIGHT, "sprites/PACMANRIGHT.png" }
    };

    Pacman pacman(pacPaths, 4, 50, 50, pacmanStartPos.x, pacmanStartPos.y, 2.0f);

    vector<Ghost*> menuGhosts = createMenuGhosts();
    vector<Ghost*> gameGhosts;

    bool inMenu = true;
    bool gameStarted = false;
    bool isPacmanSuperMode = false;
    float superModeTimer = 0.0f;
    const float PACMAN_SUPER_MODE_DURATION = 10.0f; // 10 seconds of super mode

    // Array to store ghost move timers
    float ghostMoveTimers[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float GHOST_MOVE_INTERVAL = 0.1f; // Time between ghost direction changes

    Clock clock;
    Clock gameClock; // For game time tracking

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

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

                            gameGhosts = spawnGameGhosts(window, maze);
                            cout << "Game ghosts spawned: " << gameGhosts.size() << endl;
                        }
                        else if (selectedItem == 2) {
                            window.close();
                        }
                    }
                }
                else if (gameStarted) {
                    if (event.key.code == Keyboard::Up)    pacman.SetDirection(UP);
                    if (event.key.code == Keyboard::Down)  pacman.SetDirection(DOWN);
                    if (event.key.code == Keyboard::Left)  pacman.SetDirection(LEFT);
                    if (event.key.code == Keyboard::Right) pacman.SetDirection(RIGHT);
                }
            }
        }

        window.clear(Color::Black);
        updateDots(dots);

        if (inMenu) {
            drawMenu(window, title, menuTexts, selectedItem, menuGhosts, dots, inMenu);
        }
        else if (gameStarted) {
            Vector2f nextPos = pacman.GetPosition();
            float speed = 2.0f;

            switch (pacman.GetDirection()) {
            case UP:    nextPos.y -= speed; break;
            case DOWN:  nextPos.y += speed; break;
            case LEFT:  nextPos.x -= speed; break;
            case RIGHT: nextPos.x += speed; break;
            }

            if (maze.isWalkable(nextPos))
                pacman.Move(pacman.GetDirection(), maze);
            else if (maze.isWall(pacman.GetPosition()))
                pacman.Stop(pacman.GetDirection());

            // Check if Pacman eats food
            maze.isFood(pacman.GetPosition());

            // Check if Pacman eats super food and activate super mode
            if (maze.isSuperFood(pacman.GetPosition())) {
                isPacmanSuperMode = true;
                superModeTimer = PACMAN_SUPER_MODE_DURATION;
                cout << "Pacman activated super mode!" << endl;
            }

            // Update super mode timer
            if (isPacmanSuperMode) {
                superModeTimer -= dt;
                if (superModeTimer <= 0) {
                    isPacmanSuperMode = false;
                    cout << "Pacman super mode ended!" << endl;
                }
            }

            pacman.Update();

            maze.draw(window);
            window.draw(pacman.getSprite());

            // Update and draw ghosts
            for (size_t i = 0; i < gameGhosts.size(); ++i) {
                Ghost* g = gameGhosts[i];

                // Update ghost move timer
                ghostMoveTimers[i] += dt;

                // Change direction periodically
                if (ghostMoveTimers[i] >= GHOST_MOVE_INTERVAL) {
                    Direction nextDir = getGhostDirection(g, pacman.GetPosition(), maze, isPacmanSuperMode);
                    g->Move(nextDir, maze);
                    ghostMoveTimers[i] = 0.0f;
                }

                // Update ghost animation
                g->Update(dt);

                // Handle collision with Pacman based on mode
                if (g->GhostCollision(pacman.GetPosition())) {
                    if (isPacmanSuperMode) {
                        // When Pacman is in super mode, activates ghost's super mode
                        g->SuperMode(pacman.GetPosition());
                        cout << "Ghost in super mode and returned to initial position!" << endl;
                    }
                    else {
                        // When Pacman is not in super mode
                        cout << "Pacman collided with a ghost! Game should end here." << endl;
                        // Reset the game
                        pacman.SetPosition(pacmanStartPos.x, pacmanStartPos.y);
                    }
                }

                window.draw(g->getSprite());
            }

            // Display game time
            float gameTime = gameClock.getElapsedTime().asSeconds();
            Text timeText("Time: " + to_string(int(gameTime)), font, 20);
            timeText.setPosition(20, 900);
            timeText.setFillColor(Color::White);
            window.draw(timeText);

            // Display super mode status
            if (isPacmanSuperMode) {
                Text superText("SUPER MODE: " + to_string(int(superModeTimer)), font, 20);
                superText.setPosition(windowWidth - 200, 900);
                superText.setFillColor(Color::Yellow);
                window.draw(superText);
            }
        }

        window.display();
    }

    // Clean up memory
    for (auto g : menuGhosts) delete g;
    for (auto g : gameGhosts) delete g;
}

int main() {
    MainGame();  // Run the main game loop
    return 0;
}