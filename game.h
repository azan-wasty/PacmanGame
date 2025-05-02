#pragma once
#include "help.h"
#include "Pacman.h"
#include "Ghost.h"
#include "Maze.h"

class Game {
private:
    // Window reference
    RenderWindow& window;

    // Game entities
    Pacman* pacman;
    Ghost** ghosts;
    int ghostCount;

    // Maze
    Maze* maze;

    // Game state
    int level = 0;
    int score;
    int lives;
    bool gameOver;
    bool superMode;
    Clock powerTimer;

public:
    // Constructor 
    Game(RenderWindow& win, Pacman* p, Maze* m, Ghost** g, int ghostCount) : window(win) {
        level = 1;
        score = 0;
        lives = 3;
        gameOver = false;
        superMode = false;  // Starts with no power mode
        this->ghostCount = ghostCount;  // Set ghost count dynamically

        pacman = p;
        maze = m;
        ghosts = new Ghost * [ghostCount];  // Array of pointers to Ghosts
        for (int i = 0; i < ghostCount; i++) {
            ghosts[i] = g[i];
        }
    }

    // core functionality of the game
    void display() {
        // This method would include rendering logic for the game if needed.
    }

    void CheckCollisions() {
        for (int i = 0; i < ghostCount; i++) {
            if (ghosts[i]->GhostCollison(*pacman)) {
                if (!superMode) {
                    gameOver = true;  // Game over if Pacman collides with ghost in normal mode
                }
                else {
                    ghosts[i]->update();  // Update ghost position after collision (in super mode)
                }
            }
        }

        // If Pacman hits a wall
        if (maze->isWall(pacman->GetNextPosition())) {
            pacman->Stop();  // Stop Pacman if it's about to collide with a wall
        }

        // If Pacman eats food
        if (maze->isFood(pacman->GetNextPosition())) {
            score += 10;  // Increase score
          
        }

        // If Pacman eats superfood
        if (maze->isSuperFood(pacman->GetNextPosition())) {
            superMode = true;  // This is to go even further beyond
            pacman->superSaiyan();  // Activate super mode for Pacman
            powerTimer.restart();  // Restart the power-up timer
			score += 50;  // Increase score for eating superfood
        }

        // Handle super mode timer
        if (powerTimer.getElapsedTime().asSeconds() > 5) {
            superMode = false;  // Turn off super mode after 5 seconds
            pacman->normal();  // Switch Pacman back to normal mode
        }

        // In super mode, ghosts get scared
        if (superMode) {
            for (int i = 0; i < ghostCount; i++) {
                ghosts[i]->scared();  // Make the ghosts scared
            }
        }
    }

    int HandleInput() {
        int dir;
        if (isCursorKeyPressed(dir)) {
            pacman->Move(dir);  // Move Pacman based on the direction input
        }
        return 0;
    }

    void Update() {
        // Update game entities
        pacman->Update();  // Update Pacman's position
        for (int i = 0; i < ghostCount; i++) {
            ghosts[i]->update();  // Update ghosts' positions
        }
    }

    void Run() {
        // Manages the actual running of the game and does so until Pacman and ghosts do not collide in non-super saiyan mode.
        for (int i = 0; !gameOver; i++) {

            maze->draw(window);  // Draw the maze

            pacman->Update();  // Display the movement of Pacman on the map

            score = pacman->GetScore();  // Update the score
            for (int i = 0; i < ghostCount; i++) {
                ghosts[i]->Update();  // Update ghosts
            }

            CheckCollisions();  // Check for collisions

            // If all food is eaten, end the game and reset
            if (!maze->foodremains()) {
                ResetGame();
                GameCompleted();  // Notify that the game is completed for the round
            }
        }
    }

    void ResetGame() {
        // Gets the game ready for another round in the same run
        score = 0;
        level++;  // Optionally increment the level for the next round
        maze->reset();  // Reset the maze
        ResetPositions();  // Reset the positions of Pacman and ghosts
    }

    void ResetPositions() {
        // OOP at its peak (you'll know when you see the updates of ghosts)
        pacman->resetPosition();  // Reset Pacman's position
        for (int i = 0; i < ghostCount; i++) {
            ghosts[i]->ResetPosition();  // Reset each ghost's position
        }
    }

    void GameCompleted() {
        // Handle game completion (e.g., show score, restart game)
        ShowWinScreen();
    }

    void ShowWinScreen() {
        // Display the win screen (you can design this as per your need)
        // For now, just a placeholder message
         cout << "Game Over! You completed the level!" <<  endl;
    }
};
