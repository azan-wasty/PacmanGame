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

    // Maze (2D char array inside)
    Maze* maze;

    // Game state
    int level = 0;
    int score;
    int lives;
    bool gameOver;
    bool powerMode;
    Clock powerTimer;

public:
    // Constructor 
   
        Game(RenderWindow & win,Pacman p,Maze m,Ghosts *g) : window(win) {
            level = 1;
            score = 0;
            lives = 3;
            gameOver = false;

            powerMode = false;// Starts with no power mode
            ghostCount = 4;// Game starts with 4 ghosts

            pacman =&p;
            maze = &m;
            Ghosts* ghosts[4];  // Array of pointers to Ghosts
            for (int i = 0; i < 4; i++) {
                ghosts[i] = &g[i];
            }
        }

    
    void Run() {// Manages the actual running of the code and does so until the pacman and ghost do not collide 
        // in non super saiyan mode.
       
        for (int i = 0; !gameOver; i++) {

            display();

            maze->display(level);// displays the current map

            pacman->Update();// displays the movement of pacman on the map

            score = pacman->GetScore(); // manages the score

            ghosts[i]->Update();

            if (i == 4) i = 0;

            if (ghosts[i]->GhostCollison(*pacman) && !powermode) {

                gameOver=true;
                //Game run stops if GhostCollison returns true i.e Pacman collided with the ghos in non super saiyan mode.
            }
            if (score == level_max) { // if all food on the map consumed , level up

                level++;

                if (level != max_level) {// brings the sprites back to start position
                    pacman->ResetGame();

                    for (int i = 0; i < 4; i++) {

                        ghosts[i]->ResetGame(); 
                    }
                }
                else {
                    GameCompleted();// if all levels completed game completed 
                }

                    
            }
        }
    }
    
            

    // Core functionality
    void display();
    void HandleInput(int w) {

    };
    void Update();
    virtual void ResetGame() = 0; 

    // Helpers
    void CheckCollisions();
    void ResetPositions();
    void ShowGameOver();
    void ShowWinScreen();
};
