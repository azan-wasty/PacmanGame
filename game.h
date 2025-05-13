//#pragma once
//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include "help.h"
//#include "Pacman.h"
//#include "Ghosts.h"
//#include "Maze.h"
//
//using namespace std;
//using namespace sf;
//
//class Game {
//private:
//    // Window reference
//    RenderWindow& window;
//
//    // Game entities
//    Pacman* pacman;
//    Ghost** ghosts;
//    int ghostCount;
//
//    // Maze
//    Maze* maze;
//
//    // Game state
//   
//    int score;
//    int lives;
//    bool gameOver;
//    bool superMode;
//    Clock powerTimer;
//
//public:
//    // Constructor 
//    Game(RenderWindow& win, Pacman* p, Maze* m, Ghost** g, int ghostCount) : window(win) {
//        level = 1;
//        score = 0;
//        lives = 3;
//        gameOver = false;
//        superMode = false;  // Starts with no power mode
//        this->ghostCount = ghostCount;  // Set ghost count dynamically
//
//        pacman = p;
//        maze = m;
//        ghosts = new Ghost * [ghostCount];  // Array of pointers to Ghosts
//        for (int i = 0; i < ghostCount; i++) {
//            ghosts[i] = g[i];
//        }
//    }
//
//    // core functionality of the game
//    void display() {
//        // This method would include rendering logic for the game if needed.
//    }
//
//    void CheckCollisions() {
//        for (int i = 0; i < ghostCount; i++) {
//            if (ghosts[i]->GhostCollision(pacman->GetPosition())) {
//                if (!superMode) {
//                    lives--;  // Deduct life if not in super mode
//                    if (lives <= 0)
//                        gameOver = true;  // Game over if lives run out
//                    else
//                        ResetPositions();  // Respawn positions if lives remain
//                }
//                else {
//                    ghosts[i]->Update();  // Update ghost position after collision (in super mode)
//                }
//            }
//        }
//
//        // If Pacman hits a wall
//        if (maze->isWall(pacman->GetPosition())) {
//            pacman->Stop();  // Stop Pacman if it's about to collide with a wall
//        }
//
//        // If Pacman eats food
//        if (maze->isFood(pacman->GetPosition())) {
//            score += 10;  // Increase score
//        }
//
//    //    // If Pacman eats superfood
//    //    if (maze->isSuperFood(pacman->GetNextPosition())) {
//    //        superMode = true;  // This is to go even further beyond
//    //        pacman->superSaiyan();  // Activate super mode for Pacman
//    //        powerTimer.restart();  // Restart the power-up timer
//    //        score += 50;  // Increase score for eating superfood
//    //    }
//
//    //    // Handle super mode timer
//    //    if (superMode && powerTimer.getElapsedTime().asSeconds() > 5) {
//    //        superMode = false;  // Turn off super mode after 5 seconds
//    //        pacman->normal();  // Switch Pacman back to normal mode
//    //    }
//
//    //    // In super mode, ghosts get scared
//    //    if (superMode) {
//    //        for (int i = 0; i < ghostCount; i++) {
//    //            ghosts[i]->scared();  // Make the ghosts scared
//    //        }
//    //    }
//    //}
//}
//    // Updated HandleInput method to return Direction
//   // Convert Direction enum to corresponding sf::Keyboard::Key
//    Keyboard::Key ConvertDirectionToKey(Direction dir) {
//        switch (dir) {
//        case RIGHT: return Keyboard::Right;
//        case UP: return Keyboard::Up;
//        case DOWN: return Keyboard::Down;
//        case LEFT: return Keyboard::Left;
//        default: return Keyboard::Unknown;  // Handle invalid direction
//        }
//    }
//
//    // Updated HandleInput method to return Direction
//    Direction HandleInput() {
//        Direction dir = RIGHT;  // Default to RIGHT direction (can be changed to default direction you prefer)
//
//        // Check for cursor key presses using sf::Keyboard::isKeyPressed
//        if (Keyboard::isKeyPressed(ConvertDirectionToKey(dir))) {
//            pacman->Move(dir);  // Move Pacman based on the direction input
//        }
//
//        return dir;  // Return the Direction enum value
//    }
//
//
//    void Update() {
//        // Update game entities
//        pacman->Update();  // Update Pacman's position
//        for (int i = 0; i < ghostCount; i++) {
//            ghosts[i]->Update();  // Update ghosts' positions
//        }
//    }
//
//    void Run() {
//        // Manages the actual running of the game and does so until Pacman and ghosts do not collide in non-super saiyan mode.
//        while (window.isOpen() && !gameOver) {
//
//            window.clear();  // Clear screen
//
//            HandleInput();         // Handle user input
//            Update();              // Update all game entities
//            CheckCollisions();     // Check for collisions
//            maze->draw(window);    // Draw the maze
//            window.display();      // Display everything
//
//            // If all food is eaten, end the game and reset
//            if (!maze->foodremains()) {
//                ResetGame();
//                GameCompleted();  // Notify that the game is completed for the round
//            }
//        }
//    }
//
//    void ResetGame() {
//        // Gets the game ready for another round in the same run
//        score = 0;
//        level++;  // Optionally increment the level for the next round
//        maze->reset();  // Reset the maze
//        ResetPositions();  // Reset the positions of Pacman and ghosts
//    }
//
//    void ResetPositions() {
//        // OOP at its peak (you'll know when you see the updates of ghosts)
//        pacman->Reset();  // Reset Pacman's position
//        for (int i = 0; i < ghostCount; i++) {
//            ghosts[i]->Reset();  // Reset each ghost's position
//        }
//    }
//
//    void GameCompleted() {
//        // Handle game completion (e.g., show score, restart game)
//        ShowWinScreen();
//    }
//
//    void ShowWinScreen() {
//        // Display the win screen (you can design this as per your need)
//        // For now, just a placeholder message
//        cout << "Game Over! You completed the level!" << endl;
//    }
//
//    ~Game() {
//        delete[] ghosts;  // Free ghost pointer array
//    }
//};
//
//
//
