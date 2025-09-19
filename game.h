#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <string>

using namespace std;
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "maze.h"
#include "pacman.h"
#include "Ghosts.h"
using namespace sf;

enum class GameState {
    MENU,
    INSTRUCTIONS,
    COUNTDOWN,
    PLAYING,
    LIFE_LOST,
    PACMAN_DYING,
    GAME_OVER
};

struct GhostInfo {
    string name;
    string description;
};

struct Dot {
    CircleShape shape;
    float speed;
    float angle;
    float radius;
    Vector2f center;
    float oscillation;
    bool isPulsing;
};

class Game {
private:
    // Window and graphics
    RenderWindow window;
    Font font;

    // Game state
    GameState currentState;
    int selectedMenuItem;

    // Game objects
    Maze maze;
    Pacman pacman;
    vector<Ghost*> menuGhosts;
    vector<Ghost*> gameGhosts;
    vector<string> selectedGhosts;

    // UI elements
    Text title;
    vector<Text> menuTexts;
    vector<string> menuItems;
    vector<Dot> backgroundDots;

    // Game variables
    int score;
    int lives;
    int highScore;
    bool superMode;
    float superModeTimer;
    float gameTimer;

    // Time stop ghost variables
    bool hasTimeStopGhost;
    bool pacmanFrozen;
    float nextFreezeTime;
    float freezeDuration;
    float freezeStart;

    // Ghost states for super mode
    vector<bool> ghostsBlinking;
    vector<float> ghostBlinkTimers;
    vector<Color> originalGhostColors;
    vector<bool> ghostsReturnToSpawn;

    // Countdown variables
    bool countdownActive;
    float countdownTimer;
    int countdownStage;

    // Life lost countdown variables
    bool lifeLostCountdown;
    float lifeLostTimer;

    // Pacman death variables
    bool pacmanDying;
    float pacmanDeathTimer;
    bool gameCompleted;

    // Audio
    Music menuMusic;
    Music superMusic;
    Music chomp;
    Music dead;
    Music life;

    // Constants
    static const int WINDOW_WIDTH = 960;
    static const int WINDOW_HEIGHT = 1050;
    static const float SUPER_MODE_DURATION;
    static const float COUNTDOWN_TIME_PER_STAGE;
    static const float LIFE_LOST_COUNTDOWN_DURATION;
    static const float PACMAN_DEATH_DURATION;
    static const float BLINK_RATE;

    // Ghost info map
    map<string, GhostInfo> ghostInfoMap;
    vector<string> ghostNames;

public:
    Game();
    ~Game();

    void run();

private:
    // Initialization methods
    void initializeWindow();
    void initializeFont();
    void initializeUI();
    void initializeAudio();
    void initializeGameData();
    void loadHighScore();
    void saveHighScore();

    // Game loop methods
    void handleEvents();
    void update(float dt);
    void render();

    // Event handling methods
    void handleMenuEvents(const Event& event);
    void handleGameEvents(const Event& event);
    void handleGameOverEvents(const Event& event);

    // Update methods
    void updateMenu(float dt);
    void updateCountdown(float dt);
    void updateGame(float dt);
    void updateLifeLost(float dt);
    void updatePacmanDying(float dt);
    void updateDots(float dt);
    void updateSuperMode(float dt);
    void updateTimeStop(float dt);
    void updateGhosts(float dt);

    // Render methods
    void renderMenu();
    void renderCountdown();
    void renderGame();
    void renderLifeLost();
    void renderPacmanDying();
    void renderGameOver();
    void renderUI();
    void renderCountdownText(int stage);

    // Helper methods
    void generateBackgroundDots();
    vector<Ghost*> createMenuGhosts();
    void spawnGameGhosts();
    void displayGhostAbilities();
    void displayGhostInstructions();
    void resetGame();
    void startNewGame();
    void handleCollisions();
    void checkGameWin();
    void resetGhostPositions();

    // Audio methods
    void playMenuMusic();
    void stopMenuMusic();
    void playSuperMusic();
    void stopSuperMusic();
    void playChomp();
    void stopChomp();
    void playDead();
    void stopDead();
    void playLife();
    void stopLife();

    // Utility methods
    void drawMenu();
    void drawUI();
    string getScoreMessage();
};
// Constants definition
const float Game::SUPER_MODE_DURATION = 12.0f;
const float Game::COUNTDOWN_TIME_PER_STAGE = 1.0f;
const float Game::LIFE_LOST_COUNTDOWN_DURATION = 3.0f;
const float Game::PACMAN_DEATH_DURATION = 3.0f;
const float Game::BLINK_RATE = 0.2f;

Game::Game() :
    window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pac-Man"),
    currentState(GameState::MENU),
    selectedMenuItem(0),
    score(0),
    lives(3),
    highScore(0),
    superMode(false),
    superModeTimer(0.0f),
    gameTimer(0.0f),
    hasTimeStopGhost(false),
    pacmanFrozen(false),
    nextFreezeTime(5.0f),
    freezeDuration(1.5f),
    freezeStart(0.0f),
    countdownActive(false),
    countdownTimer(0.0f),
    countdownStage(0),
    lifeLostCountdown(false),
    lifeLostTimer(0.0f),
    pacmanDying(false),
    pacmanDeathTimer(0.0f),
    gameCompleted(false)
{
    initializeWindow();
    initializeFont();
    initializeUI();
    initializeAudio();
    initializeGameData();
    loadHighScore();

    srand(static_cast<unsigned>(time(0)));
}

Game::~Game() {
    // Clean up ghosts
    for (auto ghost : menuGhosts) {
        delete ghost;
    }
    for (auto ghost : gameGhosts) {
        delete ghost;
    }
}

void Game::run() {
    Clock clock;

    playMenuMusic();

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        handleEvents();
        update(dt);
        render();
    }
}

void Game::initializeWindow() {
    window.setFramerateLimit(60);
}

void Game::initializeFont() {
    if (!font.loadFromFile("ArcadeClassic.ttf")) {
        cerr << "Error: Could not load font ArcadeClassic.ttf" << endl;
    }
}

void Game::initializeUI() {
    // Initialize title
    title.setString("PAC-MAN");
    title.setFont(font);
    title.setCharacterSize(80);
    title.setFillColor(Color::Yellow);
    title.setPosition(WINDOW_WIDTH / 2.f - title.getGlobalBounds().width / 2.f, 200);

    // Initialize menu items
    menuItems = { "Start Game", "Instructions", "Exit" };
    menuTexts.clear();

    for (size_t i = 0; i < menuItems.size(); ++i) {
        Text item(menuItems[i], font, 40);
        item.setFillColor(Color::White);
        item.setPosition(WINDOW_WIDTH / 2.f - item.getGlobalBounds().width / 2.f, 350 + i * 90);
        menuTexts.push_back(item);
    }

    generateBackgroundDots();
    menuGhosts = createMenuGhosts();
}

void Game::initializeAudio() {
    // Audio initialization is handled in play methods
}

void Game::initializeGameData() {
    // Initialize ghost info map
    ghostInfoMap = {
        {"TELEPORTER", {"TELEPORTER", "TELEPORTS - One Second here, Next Second there"}},
        {"RANDOMGHOST", {"RANDOM", "RANDOM - What's even the point ?"}},
        {"RAGE", {"RAGE", "IMMENSE BURSTS OF SPEED - Angry lil fella."}},
        {"AMBUSHER", {"AMBUSHER", "CAMPS SUPERFOOD - No Even Further Beyond"}},
        {"HERMES", {"HERMES", "SWIFT - You may hide but you will not run"}},
        {"PHANTOM", {"PHANTOM", "EXTENDED HIT RADIUS - Loves his loneliness"}},
        {"TIMESTOP", {"TIME STOP", "PAUSES PACMAN - You shall not Pass"}},
        {"RINGGHOST", {"RING", "INVISIBILITY - You can't see him (sometimes)"}}
    };

    ghostNames = {
        "TELEPORTER", "RANDOMGHOST", "RAGE", "AMBUSHER",
        "HERMES", "PHANTOM", "TIMESTOP", "RINGGHOST"
    };

    // Initialize Pacman
    Vector2i pacmanCell = maze.getP();
    Vector2f offset = maze.getOffset();
    float cellSize = Maze::getCellSize();
    Vector2f pacmanStartPos(pacmanCell.x * cellSize + offset.x, pacmanCell.y * cellSize + offset.y);

    map<Direction, string> pacPaths = {
        { UP, "sprites/PACMANUP.png" },
        { DOWN, "sprites/PACMANDOWN.png" },
        { LEFT, "sprites/PACMANLEFT.png" },
        { RIGHT, "sprites/PACMANRIGHT.png" }
    };

    pacman = Pacman(pacPaths, 4, 50, 50, pacmanStartPos.x, pacmanStartPos.y, 2.5f);

    // Initialize ghost vectors
    ghostsBlinking.resize(4, false);
    ghostBlinkTimers.resize(4, 0.0f);
    originalGhostColors.resize(4, Color::White);
    ghostsReturnToSpawn.resize(4, false);
}

void Game::loadHighScore() {
    ifstream highScoreFileIn("highscore.txt");
    if (highScoreFileIn.is_open()) {
        highScoreFileIn >> highScore;
        highScoreFileIn.close();
    }
}

void Game::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream highScoreFileOut("highscore.txt");
        if (highScoreFileOut.is_open()) {
            highScoreFileOut << highScore;
            highScoreFileOut.close();
        }
    }
}

void Game::handleEvents() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
            return;
        }

        switch (currentState) {
        case GameState::MENU:
        case GameState::INSTRUCTIONS:
            handleMenuEvents(event);
            break;
        case GameState::PLAYING:
            handleGameEvents(event);
            break;
        case GameState::GAME_OVER:
            handleGameOverEvents(event);
            break;
        default:
            break;
        }
    }
}

void Game::handleMenuEvents(const Event& event) {
    if (event.type == Event::KeyPressed) {
        if (currentState == GameState::INSTRUCTIONS) {
            if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Return) {
                currentState = GameState::MENU;
            }
        }
        else {
            if (event.key.code == Keyboard::Up) {
                selectedMenuItem = (selectedMenuItem - 1 + menuItems.size()) % menuItems.size();
            }
            else if (event.key.code == Keyboard::Down) {
                selectedMenuItem = (selectedMenuItem + 1) % menuItems.size();
            }
            else if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Return) {
                if (selectedMenuItem == 0) {
                    startNewGame();
                }
                else if (selectedMenuItem == 1) {
                    currentState = GameState::INSTRUCTIONS;
                }
                else if (selectedMenuItem == 2) {
                    window.close();
                }
            }
        }
    }
}

void Game::handleGameEvents(const Event& event) {
    if (event.type == Event::KeyPressed && !countdownActive && !lifeLostCountdown && !pacmanDying) {
        switch (event.key.code) {
        case Keyboard::Up:    pacman.SetDirection(UP); break;
        case Keyboard::Down:  pacman.SetDirection(DOWN); break;
        case Keyboard::Left:  pacman.SetDirection(LEFT); break;
        case Keyboard::Right: pacman.SetDirection(RIGHT); break;
        case Keyboard::S:     // Debug super mode
            superMode = true;
            superModeTimer = SUPER_MODE_DURATION;
            for (auto g : gameGhosts) {
                g->setColor(Color::White);
            }
            break;
        default: break;
        }
    }
}

void Game::handleGameOverEvents(const Event& event) {
    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Return) {
            resetGame();
        }
    }
}

void Game::update(float dt) {
    updateDots(dt);

    switch (currentState) {
    case GameState::MENU:
        updateMenu(dt);
        break;
    case GameState::COUNTDOWN:
        updateCountdown(dt);
        break;
    case GameState::PLAYING:
        updateGame(dt);
        break;
    case GameState::LIFE_LOST:
        updateLifeLost(dt);
        break;
    case GameState::PACMAN_DYING:
        updatePacmanDying(dt);
        break;
    default:
        break;
    }
}

void Game::updateMenu(float dt) {
    // Move and update menu ghosts
    for (size_t i = 0; i < menuGhosts.size(); ++i) {
        Ghost* g = menuGhosts[i];
        g->menMove(RIGHT);
        g->Update(dt);

        if (g->GetPosition().x > WINDOW_WIDTH) {
            float newX = -190.f - (rand() % 100);
            float newY = 640.f + (rand() % 80);
            g->SetPosition(newX, newY);
        }
    }
}

void Game::updateCountdown(float dt) {
    countdownTimer += dt;

    if (countdownTimer >= COUNTDOWN_TIME_PER_STAGE) {
        countdownStage++;
        countdownTimer = 0.0f;

        if (countdownStage > 2) {
            countdownActive = false;
            currentState = GameState::PLAYING;
        }
    }
}

void Game::updateGame(float dt) {
    gameTimer += dt;

    updateSuperMode(dt);
    updateTimeStop(dt);

    // Update Pacman
    if (!pacmanFrozen) {
        Vector2f nextPos = pacman.GetPosition();
        float speed = 2.0f;

        switch (pacman.GetDirection()) {
        case UP:    nextPos.y -= speed; break;
        case DOWN:  nextPos.y += speed; break;
        case LEFT:  nextPos.x -= speed; break;
        case RIGHT: nextPos.x += speed; break;
        }

        if (maze.isWalkable(nextPos)) {
            pacman.Move(pacman.GetDirection(), maze);
        }
        else if (!maze.isWalkable(pacman.GetPosition())) {
            pacman.Stop(pacman.GetDirection());
        }

        // Food collection
        if (maze.isFood(pacman.GetPosition())) {
            score += 10;
            playChomp();
        }

        if (maze.isSuperFood(pacman.GetPosition())) {
            score += 50;
            superMode = true;
            pacman.SuperScale();
            superModeTimer = SUPER_MODE_DURATION;
            playSuperMusic();

            for (auto g : gameGhosts) {
                g->setColor(Color::White);
            }
        }

        pacman.Update();
    }

    updateGhosts(dt);
    handleCollisions();
    checkGameWin();
}

void Game::updateLifeLost(float dt) {
    lifeLostTimer += dt;
    playLife();
    chomp.setVolume(0);

    if (lifeLostTimer >= LIFE_LOST_COUNTDOWN_DURATION) {
        lifeLostCountdown = false;
        lifeLostTimer = 0.0f;
        stopLife();
        chomp.setVolume(15);

        if (lives <= 0) {
            currentState = GameState::PACMAN_DYING;
            pacmanDeathTimer = 0.0f;
        }
        else {
            currentState = GameState::PLAYING;
        }
    }
}

void Game::updatePacmanDying(float dt) {
    pacmanDeathTimer += dt;

    // Make Pacman blink and fade
    if ((int)(pacmanDeathTimer / BLINK_RATE) % 2 == 0) {
        int alpha = 255 * (1.0f - (pacmanDeathTimer / PACMAN_DEATH_DURATION));
        alpha = max(0, min(255, alpha));
        pacman.setColor(Color(255, 255, 0, alpha));
    }
    else {
        pacman.setColor(Color(255, 255, 0, 0));
        stopChomp();
    }

    playDead();

    if (pacmanDeathTimer >= PACMAN_DEATH_DURATION) {
        stopChomp();
        stopDead();
        pacman.setColor(Color(255, 255, 0, 255));
        saveHighScore();
        currentState = GameState::GAME_OVER;
    }
}

void Game::updateDots(float dt) {
    for (auto& d : backgroundDots) {
        if (d.radius > 0) {
            // Orbital dot
            d.angle += d.speed * dt;
            float x = d.center.x + d.radius * cos(d.angle);
            float y = d.center.y + d.radius * sin(d.angle);
            d.shape.setPosition(x, y);

            if (d.isPulsing) {
                d.oscillation += dt * 3.0f;
                float scale = 0.7f + 0.3f * sin(d.oscillation);
                float currentRadius = d.shape.getRadius();
                d.shape.setRadius(currentRadius * scale);

                float brightness = 150 + 105 * sin(d.oscillation);
                d.shape.setFillColor(Color(255, 255, brightness, 200));
            }
        }
        else {
            // Moving dot
            Vector2f pos = d.shape.getPosition();
            pos.y += d.speed;
            if (pos.y > WINDOW_HEIGHT) pos.y = 0;
            d.shape.setPosition(pos);
        }
    }
}

void Game::updateSuperMode(float dt) {
    if (superMode) {
        chomp.setVolume(0);
        superModeTimer -= dt;
        if (superModeTimer <= 0) {
            superMode = false;
            for (size_t i = 0; i < gameGhosts.size() && i < originalGhostColors.size(); i++) {
                if (!ghostsBlinking[i] && !ghostsReturnToSpawn[i]) {
                    gameGhosts[i]->setColor(Color::White);
                }
            }
        }
    }

    if (!superMode) {
        pacman.ResetScale();
        stopSuperMusic();
        chomp.setVolume(15);
    }
}

void Game::updateTimeStop(float dt) {
    if (hasTimeStopGhost && !pacmanFrozen && gameTimer >= nextFreezeTime) {
        pacmanFrozen = true;
        freezeStart = gameTimer;
        nextFreezeTime = gameTimer + 25.0f;
        pacman.Stop(pacman.GetDirection());
    }

    if (pacmanFrozen && (gameTimer - freezeStart >= freezeDuration)) {
        pacmanFrozen = false;
    }
}

void Game::updateGhosts(float dt) {
    for (size_t i = 0; i < gameGhosts.size() && i < ghostsBlinking.size(); i++) {
        Ghost* g = gameGhosts[i];

        if (ghostsBlinking[i]) {
            ghostBlinkTimers[i] += dt;

            if (static_cast<int>(ghostBlinkTimers[i] * 5) % 2 == 0) {
                g->setColor(Color::White);
            }
            else {
                g->setColor(Color(255, 255, 255, 50));
            }

            if (ghostBlinkTimers[i] >= 2.0f) {
                ghostsBlinking[i] = false;
                ghostsReturnToSpawn[i] = true;
                g->setColor(originalGhostColors[i]);

                Vector2i spawnPos = maze.getGhost('0');
                float cellSize = Maze::getCellSize();
                g->SetPosition(
                    spawnPos.x * cellSize + cellSize / 2,
                    spawnPos.y * cellSize + cellSize / 2
                );
                ghostsReturnToSpawn[i] = false;
            }
        }
        else if (!ghostsReturnToSpawn[i]) {
            g->updateAutonomous(maze);
        }

        g->Update(dt);
    }
}

void Game::render() {
    window.clear(Color::Black);

    switch (currentState) {
    case GameState::MENU:
        renderMenu();
        break;
    case GameState::INSTRUCTIONS:
        displayGhostInstructions();
        break;
    case GameState::COUNTDOWN:
        renderCountdown();
        break;
    case GameState::PLAYING:
        renderGame();
        break;
    case GameState::LIFE_LOST:
        renderLifeLost();
        break;
    case GameState::PACMAN_DYING:
        renderPacmanDying();
        break;
    case GameState::GAME_OVER:
        renderGameOver();
        break;
    }

    window.display();
}

void Game::renderMenu() {
    drawMenu();
}

void Game::renderCountdown() {
    maze.draw(window);
    renderCountdownText(countdownStage);
}

void Game::renderGame() {
    maze.draw(window);

    for (auto g : gameGhosts) {
        window.draw(g->getSprite());
    }

    window.draw(pacman.getSprite());
    renderUI();
}

void Game::renderLifeLost() {
    maze.draw(window);

    window.draw(pacman.getSprite());
    for (auto g : gameGhosts) {
        window.draw(g->getSprite());
    }

    renderUI();

    Text lifeLostText("LIFE LOST", font, 40);
    lifeLostText.setFillColor(Color::Red);
    lifeLostText.setPosition(WINDOW_WIDTH / 2.f - lifeLostText.getGlobalBounds().width / 2.f, 340);
    window.draw(lifeLostText);

    Text countdownText(to_string((int)(LIFE_LOST_COUNTDOWN_DURATION - lifeLostTimer) + 1), font, 80);
    countdownText.setFillColor(Color::Yellow);
    countdownText.setPosition(WINDOW_WIDTH / 2.f - countdownText.getGlobalBounds().width / 2.f, 400);
    window.draw(countdownText);
}

void Game::renderPacmanDying() {
    maze.draw(window);
    renderUI();

    for (auto g : gameGhosts) {
        window.draw(g->getSprite());
    }

    window.draw(pacman.getSprite());
}

void Game::renderGameOver() {
    // Draw background dots
    for (auto& d : backgroundDots) {
        window.draw(d.shape);
    }

    Text gameOverText("GAME OVER", font, 70);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setPosition(WINDOW_WIDTH / 2.f - gameOverText.getGlobalBounds().width / 2.f, 300);
    window.draw(gameOverText);

    Text scoreText("SCORE: " + to_string(score), font, 100);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition((WINDOW_WIDTH / 2.f - gameOverText.getGlobalBounds().width / 2.f) - 130, 380);
    window.draw(scoreText);

    Text highScoreText("HIGH SCORE: " + to_string(highScore), font, 50);
    highScoreText.setFillColor(Color::Yellow);
    highScoreText.setPosition(WINDOW_WIDTH / 2.f - highScoreText.getGlobalBounds().width / 2.f, 550);
    window.draw(highScoreText);

    // Score message
    Text messageText(getScoreMessage(), font, 40);
    messageText.setFillColor(Color::White);
    messageText.setOrigin(messageText.getLocalBounds().left + messageText.getLocalBounds().width / 2.f,
        messageText.getLocalBounds().top + messageText.getLocalBounds().height / 2.f);
    messageText.setPosition(WINDOW_WIDTH / 2.f, 520);
    window.draw(messageText);

    // Flashing return text
    static float flashTimer = 0.0f;
    flashTimer += 0.016f; // Approximate 60 FPS
    if (sin(flashTimer * 3.0f) > 0) {
        Text pressEnter("PRESS ENTER TO RETURN TO MENU", font, 30);
        pressEnter.setFillColor(Color::White);
        pressEnter.setPosition(WINDOW_WIDTH / 2.f - pressEnter.getGlobalBounds().width / 2.f, 650);
        window.draw(pressEnter);
    }
}

void Game::renderUI() {
    drawUI();
}

void Game::renderCountdownText(int stage) {
    Text countdownText;
    countdownText.setFont(font);
    countdownText.setCharacterSize(72);
    countdownText.setFillColor(Color::Yellow);

    switch (stage) {
    case 0: countdownText.setString("Ready"); break;
    case 1: countdownText.setString("Set"); break;
    case 2: countdownText.setString("Go!"); break;
    default: return;
    }

    FloatRect textBounds = countdownText.getLocalBounds();
    countdownText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    countdownText.setPosition(window.getSize().x / 2, window.getSize().y / 2 - 15);
    window.draw(countdownText);
}

void Game::generateBackgroundDots() {
    backgroundDots.clear();

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    uniform_real_distribution<float> radiusDist(100.0f, 350.0f);
    uniform_real_distribution<float> speedDist(0.5f, 2.5f);
    uniform_real_distribution<float> oscDist(0.0f, 2.0f * 3.14159f);
    bernoulli_distribution isPulsingDist(0.3f);

    // Standard moving dots
    for (int i = 0; i < 50; ++i) {
        CircleShape dot(2 + static_cast<float>(rand() % 3));

        if (rand() % 5 == 0) {
            dot.setFillColor(Color(rand() % 255, rand() % 255, rand() % 255, 150 + rand() % 100));
        }
        else {
            dot.setFillColor(Color(200, 200, 200, 150 + rand() % 100));
        }

        dot.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
        float speed = speedDist(gen);
        backgroundDots.push_back({ dot, speed, 0, 0, {0, 0}, 0, false });
    }

    // Orbital dots
    for (int i = 0; i < 30; ++i) {
        CircleShape dot(1 + static_cast<float>(rand() % 2));
        dot.setFillColor(Color::Yellow);

        Vector2f center(WINDOW_WIDTH / 2.0f, 300.0f);
        float radius = radiusDist(gen);
        float angle = angleDist(gen);
        float speed = speedDist(gen) * 0.5f;

        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        dot.setPosition(x, y);

        bool isPulsing = isPulsingDist(gen);
        float oscillation = oscDist(gen);

        backgroundDots.push_back({ dot, speed, angle, radius, center, oscillation, isPulsing });
    }
}

vector<Ghost*> Game::createMenuGhosts() {
    random_device rd;
    mt19937 gen(rd());
    shuffle(ghostNames.begin(), ghostNames.end(), gen);

    vector<Ghost*> ghosts;
    if (ghostNames.size() < 4) {
        cerr << "Not enough ghost names to create menu ghosts!" << endl;
        return ghosts;
    }

    for (float i = 0; i < 4; ++i) {
        string ghostName = ghostNames[i];
        string spriteSheetPath = "sprites/" + ghostName + ".png";
        map<Direction, int> frameIndexes = {
            {RIGHT, 0}, {UP, 1}, {DOWN, 2}, {LEFT, 3}
        };

        float x = -120.0f - (i * 150.0f);
        float y = 640.0f + fmod(i * 30.0f, 80.0f);
        float ghostSpeed = 2.0f + (i * 0.5f);

        Ghost* g = new Ghost(spriteSheetPath, 4, 50, 50, x, y, ghostSpeed, 5.0f, frameIndexes);
        ghosts.push_back(g);
    }
    return ghosts;
}

void Game::spawnGameGhosts() {
    vector<string> availableGhosts = {
        "RANDOMGHOST", "RAGE", "AMBUSHER", "PHANTOM",
        "HERMES", "RINGGHOST", "TELEPORTER", "TIMESTOP"
    };

    random_device rd;
    mt19937 gen(rd());
    shuffle(availableGhosts.begin(), availableGhosts.end(), gen);

    selectedGhosts.clear();
    for (int i = 0; i < 4; ++i) {
        selectedGhosts.push_back(availableGhosts[i]);
    }

    hasTimeStopGhost = false;

    for (int i = 0; i < 4; ++i) {
        string ghostName = availableGhosts[i];
        string spriteSheetPath = "sprites/" + ghostName + ".png";

        map<Direction, int> frameIndexes = {
            {RIGHT, 0}, {UP, 1}, {DOWN, 2}, {LEFT, 3}
        };

        Vector2i ghostPos = maze.getGhost(i + '0');
        if (ghostPos.x == -1 || ghostPos.y == -1) continue;

        static const int TILE_SIZE = 40;
        float x = ghostPos.x * TILE_SIZE;
        float y = ghostPos.y * TILE_SIZE;

        Ghost* g = nullptr;

        if (ghostName == "HERMES") {
            g = new Ghost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 3.5f, 1.3f, frameIndexes);
        }
        else if (ghostName == "RINGGHOST") {
            g = new RingGhost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }
        else if (ghostName == "TELEPORTER") {
            g = new TeleporterGhost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }
        else if (ghostName == "PHANTOM") {
            g = new Ghost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }
        else if (ghostName == "AMBUSHER") {
            g = new AmbusherGhost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }
        else if (ghostName == "TIMESTOP") {
            g = new TimeStopGhost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
            hasTimeStopGhost = true;
        }
        else if (ghostName == "RAGE") {
            g = new ChaserGhost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }
        else {
            g = new Ghost(spriteSheetPath, 4, 50, 50, x + TILE_SIZE / 2, y + TILE_SIZE / 2, 2.5f, 1.3f, frameIndexes);
        }

        if (g) {
            gameGhosts.push_back(g);
        }
    }

    // Store original ghost colors
    for (size_t i = 0; i < gameGhosts.size() && i < originalGhostColors.size(); i++) {
        originalGhostColors[i] = gameGhosts[i]->getSprite().getColor();
    }
}

void Game::displayGhostAbilities() {
    Clock displayClock;
    float displayTime = 0.0f;
    const float DISPLAY_DURATION = 10.0f;

    while (displayTime < DISPLAY_DURATION && window.isOpen()) {
        displayTime = displayClock.getElapsedTime().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return;
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Return || event.key.code == Keyboard::Space) {
                    return;
                }
            }
        }

        window.clear(Color(0, 0, 0));

        updateDots(0.016f);
        for (auto& d : backgroundDots) {
            window.draw(d.shape);
        }

        Text title("BEWARE OF THESE GHOSTS!", font, 48);
        title.setFillColor(Color::Yellow);
        title.setPosition(WINDOW_WIDTH / 2.f - title.getGlobalBounds().width / 2.f, 110);
        window.draw(title);

        for (int i = 0; i < selectedGhosts.size() && i < 4; ++i) {
            string ghostType = selectedGhosts[i];

            Sprite ghostSprite;
            Texture ghostTexture;
            if (ghostTexture.loadFromFile("sprites/" + ghostType + ".png")) {
                ghostSprite.setTexture(ghostTexture);
                ghostSprite.setTextureRect(IntRect(0, 0, 50, 50));
                ghostSprite.setScale(2.0f, 2.0f);
                ghostSprite.setPosition(30, 220 + i * 170);
                window.draw(ghostSprite);
            }

            Text nameText(ghostInfoMap[ghostType].name, font, 36);
            nameText.setFillColor(Color::Cyan);
            nameText.setPosition(110, 210 + i * 160);
            window.draw(nameText);

            Text descText(ghostInfoMap[ghostType].description, font, 24);
            descText.setFillColor(Color(200, 200, 200));
            descText.setPosition(150, 250 + i * 170);
            window.draw(descText);
        }

        Text skipText("PRESS ENTER OR SPACE TO CONTINUE", font, 24);
        skipText.setFillColor(Color(150, 150, 150));
        skipText.setPosition(WINDOW_WIDTH / 2.f - skipText.getGlobalBounds().width / 2.f, WINDOW_HEIGHT - 100);

        if (static_cast<int>(displayTime * 2) % 2 == 0) {
            window.draw(skipText);
        }

        window.display();
    }
}

void Game::displayGhostInstructions() {
    // Draw background
    for (auto& d : backgroundDots) {
        window.draw(d.shape);
    }

    Text title("KNOW YOUR ENEMIES", font, 36);
    title.setFillColor(Color::Red);
    title.setPosition(WINDOW_WIDTH / 2.f - title.getGlobalBounds().width / 2.f, 40);
    window.draw(title);

    Text subtitle("GHOST ABILITIES", font, 28);
    subtitle.setFillColor(Color::White);
    subtitle.setPosition(WINDOW_WIDTH / 2.f - subtitle.getGlobalBounds().width / 2.f, 80);
    window.draw(subtitle);

    const int ROW_HEIGHT = 90;
    const int START_Y = 120;

    for (int i = 0; i < ghostNames.size(); ++i) {
        string ghostType = ghostNames[i];
        float x = 30;
        float y = START_Y + i * ROW_HEIGHT;

        Sprite ghostSprite;
        Texture ghostTexture;
        if (ghostTexture.loadFromFile("sprites/" + ghostType + ".png")) {
            ghostSprite.setTexture(ghostTexture);
            ghostSprite.setTextureRect(IntRect(0, 0, 50, 50));
            ghostSprite.setScale(1.0f, 1.0f);
            ghostSprite.setPosition(x, y);
            window.draw(ghostSprite);
        }

        Text nameText(ghostInfoMap[ghostType].name, font, 22);
        nameText.setFillColor(Color::Cyan);
        nameText.setPosition(x + 60, y);
        window.draw(nameText);

        Text descText(ghostInfoMap[ghostType].description, font, 16);
        descText.setFillColor(Color(200, 200, 200));
        descText.setPosition(x + 60, y + 25);
        window.draw(descText);
    }

    Text superModeTitle("SUPER MODE", font, 28);
    superModeTitle.setFillColor(Color::Yellow);
    superModeTitle.setPosition(WINDOW_WIDTH / 2.f - superModeTitle.getGlobalBounds().width / 2.f, WINDOW_HEIGHT - 170);
    window.draw(superModeTitle);

    Text superModeText("EAT YELLOW SUPER FOOD TO GO EVEN FURTHER BEYOND,", font, 18);
    superModeText.setFillColor(Color::Yellow);
    superModeText.setPosition(WINDOW_WIDTH / 2.f - superModeText.getGlobalBounds().width / 2.f, WINDOW_HEIGHT - 140);
    window.draw(superModeText);

    Text superModeText2("SEND GHOSTS BACK AND INCREASE SPEED", font, 18);
    superModeText2.setFillColor(Color::Yellow);
    superModeText2.setPosition(WINDOW_WIDTH / 2.f - superModeText2.getGlobalBounds().width / 2.f, WINDOW_HEIGHT - 120);
    window.draw(superModeText2);

    Text skipText("PRESS ENTER TO CONTINUE", font, 20);
    skipText.setFillColor(Color(150, 150, 150));
    skipText.setPosition(WINDOW_WIDTH / 2.f - skipText.getGlobalBounds().width / 2.f, WINDOW_HEIGHT - 50);
    window.draw(skipText);
}

void Game::resetGame() {
    currentState = GameState::MENU;
    score = 0;
    lives = 3;
    superMode = false;
    superModeTimer = 0.0f;
    gameTimer = 0.0f;
    hasTimeStopGhost = false;
    pacmanFrozen = false;
    gameCompleted = false;

    // Clean up game ghosts
    for (auto ghost : gameGhosts) {
        delete ghost;
    }
    gameGhosts.clear();
    selectedGhosts.clear();

    // Reset maze
    maze.reset();

    // Reset Pacman
    Vector2i pacmanCell = maze.getP();
    Vector2f offset = maze.getOffset();
    float cellSize = Maze::getCellSize();
    Vector2f pacmanStartPos(pacmanCell.x * cellSize + offset.x, pacmanCell.y * cellSize + offset.y);
    pacman.SetPosition(pacmanStartPos.x, pacmanStartPos.y);
    pacman.setColor(Color(255, 255, 0, 255));

    // Reset ghost state vectors
    fill(ghostsBlinking.begin(), ghostsBlinking.end(), false);
    fill(ghostBlinkTimers.begin(), ghostBlinkTimers.end(), 0.0f);
    fill(ghostsReturnToSpawn.begin(), ghostsReturnToSpawn.end(), false);

    playMenuMusic();
}

void Game::startNewGame() {
    currentState = GameState::COUNTDOWN;
    countdownActive = true;
    countdownTimer = 0.0f;
    countdownStage = 0;

    stopMenuMusic();

    spawnGameGhosts();
    displayGhostAbilities();

    // Reset Pacman position
    Vector2i pacmanCell = maze.getP();
    Vector2f offset = maze.getOffset();
    float cellSize = Maze::getCellSize();
    Vector2f pacmanStartPos(pacmanCell.x * cellSize + offset.x, pacmanCell.y * cellSize + offset.y);
    pacman.SetPosition(pacmanStartPos.x, pacmanStartPos.y);
}

void Game::handleCollisions() {
    for (size_t i = 0; i < gameGhosts.size() && i < ghostsBlinking.size(); i++) {
        Ghost* g = gameGhosts[i];

        if (!ghostsBlinking[i] && !ghostsReturnToSpawn[i]) {
            if (g->GhostCollision(pacman.GetPosition())) {
                if (superMode) {
                    score += 200;
                    ghostsBlinking[i] = true;
                    ghostBlinkTimers[i] = 0.0f;
                }
                else {
                    lives--;
                    currentState = GameState::LIFE_LOST;
                    lifeLostCountdown = true;
                    lifeLostTimer = 0.0f;
                    resetGhostPositions();
                    break;
                }
            }
        }
    }
}

void Game::checkGameWin() {
    if (!maze.foodremains()) {
        gameCompleted = true;
        saveHighScore();
        currentState = GameState::GAME_OVER;
    }
}

void Game::resetGhostPositions() {
    float cellSize = Maze::getCellSize();

    for (size_t j = 0; j < gameGhosts.size(); j++) {
        char ghostId = '0' + j;
        Vector2i spawnPos;
        if (j < 4) {
            spawnPos = maze.getGhost(ghostId);
        }
        else {
            spawnPos = maze.getGhost('0');
        }

        gameGhosts[j]->SetPosition(
            (spawnPos.x * cellSize + cellSize / 2) + 20,
            (spawnPos.y * cellSize + cellSize / 2) + 20
        );

        ghostsBlinking[j] = false;
        ghostsReturnToSpawn[j] = false;
        if (j < originalGhostColors.size()) {
            gameGhosts[j]->setColor(originalGhostColors[j]);
        }
    }

    // Reset Pacman position
    Vector2i pacmanCell = maze.getP();
    Vector2f offset = maze.getOffset();
    cellSize = Maze::getCellSize();
    Vector2f pacmanStartPos(pacmanCell.x * cellSize + offset.x, pacmanCell.y * cellSize + offset.y);
    pacman.SetPosition(pacmanStartPos.x, pacmanStartPos.y);
}

void Game::playMenuMusic() {
    if (!menuMusic.openFromFile("sounds/pm.ogg")) {
        cerr << "Error: Could not load pm.ogg\n";
        return;
    }
    menuMusic.setLoop(true);
    menuMusic.setVolume(50);
    menuMusic.play();
}

void Game::stopMenuMusic() {
    if (menuMusic.getStatus() == Music::Playing) {
        menuMusic.stop();
    }
}

void Game::playSuperMusic() {
    if (!superMusic.openFromFile("sounds/ssj3.ogg")) {
        cerr << "Error: Could not load ssj3.ogg\n";
        return;
    }
    menuMusic.setVolume(75);
    chomp.setVolume(0);
    superMusic.play();
}

void Game::stopSuperMusic() {
    if (superMusic.getStatus() == Music::Playing) {
        superMusic.stop();
        chomp.setVolume(15);
    }
}

void Game::playChomp() {
    if (!chomp.openFromFile("sounds/chomp.wav")) {
        cerr << "Error: Could not load chomp.wav\n";
        return;
    }
    chomp.setLoop(true);
    chomp.setVolume(15);
    chomp.play();
}

void Game::stopChomp() {
    if (chomp.getStatus() == Music::Playing) {
        chomp.stop();
    }
}

void Game::playDead() {
    if (!dead.openFromFile("sounds/dead.wav")) {
        cerr << "Error: Could not load dead.wav\n";
        return;
    }
    dead.setLoop(true);
    dead.setVolume(25);
    dead.play();
}

void Game::stopDead() {
    if (dead.getStatus() == Music::Playing) {
        dead.stop();
    }
}

void Game::playLife() {
    if (!life.openFromFile("sounds/life.ogg")) {
        cerr << "Error: Could not load life.ogg\n";
        return;
    }
    life.setLoop(true);
    life.setVolume(25);
    life.play();
}

void Game::stopLife() {
    if (life.getStatus() == Music::Playing) {
        life.stop();
    }
}

void Game::drawMenu() {
    // Draw background dots
    for (auto& d : backgroundDots) {
        window.draw(d.shape);
    }

    // Title pulsing effect
    static float titlePulseTimer = 0.0f;
    titlePulseTimer += 0.016f;
    float titleScale = 1.0f + 0.05f * sin(titlePulseTimer * 3.0f);
    title.setScale(titleScale, titleScale);

    int r = 255;
    int g = 255;
    int b = static_cast<int>(60 + 40 * sin(titlePulseTimer * 2.0f));
    title.setFillColor(Color(r, g, b));

    // Draw title with shadow
    Text shadowTitle = title;
    shadowTitle.setFillColor(Color(30, 30, 30, 150));
    shadowTitle.setPosition(title.getPosition() + Vector2f(3, 3));
    window.draw(shadowTitle);
    window.draw(title);

    // Draw menu options
    for (size_t i = 0; i < menuTexts.size(); ++i) {
        Color baseColor = (i == selectedMenuItem) ? Color::Yellow : Color::White;

        if (i == selectedMenuItem) {
            float pulseValue = 0.7f + 0.3f * sin(titlePulseTimer * 5.0f);
            baseColor = Color(
                static_cast<Uint8>(255 * pulseValue),
                static_cast<Uint8>(255 * pulseValue),
                static_cast<Uint8>(50)
            );

            menuTexts[i].setScale(1.1f, 1.1f);

            // Arrow cursor
            Text arrow(">>", font, 40);
            arrow.setFillColor(baseColor);
            arrow.setPosition(
                menuTexts[i].getPosition().x - arrow.getGlobalBounds().width - 15,
                menuTexts[i].getPosition().y
            );
            window.draw(arrow);
        }
        else {
            menuTexts[i].setScale(1.0f, 1.0f);
        }

        menuTexts[i].setFillColor(baseColor);

        // Draw shadow
        Text shadowText = menuTexts[i];
        shadowText.setFillColor(Color(30, 30, 30, 150));
        shadowText.setPosition(menuTexts[i].getPosition() + Vector2f(2, 2));
        shadowText.setScale(menuTexts[i].getScale());
        window.draw(shadowText);
        window.draw(menuTexts[i]);
    }

    // Draw menu ghosts
    if (currentState == GameState::MENU) {
        for (auto g : menuGhosts) {
            window.draw(g->getSprite());
        }

        // Flashing "Press Enter" text
        static float flashTimer = 0.0f;
        flashTimer += 0.016f;
        if (sin(flashTimer * 3.0f) > 0) {
            Text pressEnter("PRESS ENTER TO START", font, 30);
            pressEnter.setFillColor(Color::White);
            pressEnter.setPosition(WINDOW_WIDTH / 2.f - pressEnter.getGlobalBounds().width / 2.f, 650);

            Text shadowPressEnter = pressEnter;
            shadowPressEnter.setFillColor(Color(30, 30, 30, 150));
            shadowPressEnter.setPosition(pressEnter.getPosition() + Vector2f(2, 2));
            window.draw(shadowPressEnter);
            window.draw(pressEnter);
        }
    }
}

void Game::drawUI() {
    Text highscoreText("HIGHSCORE: " + to_string(highScore), font, 30);
    highscoreText.setFillColor(Color::Yellow);
    highscoreText.setPosition(325, 40);

    Text highScore = highscoreText;
    highScore.setFillColor(Color(30, 30, 30, 150));
    highScore.setPosition(highscoreText.getPosition() + Vector2f(2, 2));
    window.draw(highScore);
    window.draw(highscoreText);

    Text scoreText("SCORE: " + to_string(score), font, 30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(50, 900);

    Text shadowScore = scoreText;
    shadowScore.setFillColor(Color(30, 30, 30, 150));
    shadowScore.setPosition(scoreText.getPosition() + Vector2f(2, 2));
    window.draw(shadowScore);
    window.draw(scoreText);

    Text livesText("LIVES: " + to_string(lives), font, 30);
    livesText.setFillColor(Color::White);
    livesText.setPosition(WINDOW_WIDTH - livesText.getGlobalBounds().width - 50, 900);

    Text shadowLives = livesText;
    shadowLives.setFillColor(Color(30, 30, 30, 150));
    shadowLives.setPosition(livesText.getPosition() + Vector2f(2, 2));
    window.draw(shadowLives);
    window.draw(livesText);

    if (superMode) {
        int timerSeconds = static_cast<int>(superModeTimer);
        Text superText("SUPER MODE: " + to_string(timerSeconds), font, 30);
        superText.setFillColor(Color::Yellow);
        superText.setPosition(WINDOW_WIDTH / 2.f - superText.getGlobalBounds().width / 2.f, 930);

        static float pulseTimer = 0.0f;
        pulseTimer += 0.1f;
        float scale = 1.0f + 0.1f * sin(pulseTimer * 5.0f);
        superText.setScale(scale, scale);

        Text shadowSuper = superText;
        shadowSuper.setFillColor(Color(30, 30, 30, 150));
        shadowSuper.setPosition(superText.getPosition() + Vector2f(2, 2));
        shadowSuper.setScale(superText.getScale());
        window.draw(shadowSuper);
        window.draw(superText);
    }
}

string Game::getScoreMessage() {
    if (gameCompleted) {
        return "COMPLETED LESSGOO";
    }
    else if (score == 0) {
        return "That was intentional right ?";
    }
    else if (score > 0 && score < 1000) {
        return "You're getting there?";
    }
    else if (score > 999 && score < 2000) {
        return "Pretty Impressive huh";
    }
    else if (score > 1999 && score < 3000) {
        return gameCompleted ? "COMPLETED LESSGOO" : "Almost Completed Huh";
    }
    else if (score > 2999 && score < 4000) {
        return gameCompleted ? "COMPLETED LESSGOO" : "How'd you not win?";
    }
    else if (score > 4000) {
        return gameCompleted ? "HOW DID YOU GET 4K+" : "HOW DID YOU GET 4K+,,and lost????";
    }
    return "";
}
