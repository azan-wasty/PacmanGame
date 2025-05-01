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
    bool superMode;
    Clock powerTimer;

public:
    // Constructor 

    Game(RenderWindow& win, Pacman p, Maze m, Ghosts** g) : window(win) {
        level = 1;
        score = 0;
        lives = 3;
        gameOver = false;

        superMode = false; // Starts with no power mode
        ghostCount = 4; // Game starts with 4 ghosts

        pacman = &p;
        maze = &m;
        ghosts = new Ghost*[ghostCount];  // Array of pointers to Ghosts
        for (int i = 0; i < ghostCount; i++) {
            ghosts[i] = g[i];
        }
    }
	// core functionality of the game
    void display();
    void CheckCollisions() {
		for (int i = 0; i < ghostCount; i++) {
			if (ghosts[i]->GhostCollison(*pacman)) {
				if (!superMode) {
					gameOver = true;
				}
                else {
                    ghosts[i]->update();
                }
			}
		}
        if (!maze->IsWall(pacman->GetNextPosition())) {
            pacman->MoveInCurrentDirection();
        }


    }
    int HandleInput() {
        int dir;
        if (isCursorKeyPressed(dir)) {
            return dir;
        }
        return 0;
    }
	void Update() {
		// Update the game state
		pacman->Update();
		for (int i = 0; i < ghostCount; i++) {
			ghosts[i]->Death();
		}
        if (maze->foodconsumed()) {

        }
	}


    void Run() {// Manages the actual running of the code and does so until the pacman and ghost do not collide 
        // in non super saiyan mode.

        for (int i = 0; !gameOver; i++) {
            display();
            maze->display(level);// displays the map

            pacman->Update();// displays the movement of pacman on the map
            if (superMode) {
                ghosts[i]->fear();
            }

            score = pacman->GetScore(); // manages the score
            ghosts[i]->Update();
			CheckCollisions(); //Game run stops if GhostCollison returns true i.e Pacman collided with the ghos in non super saiyan mode.
            
            if (score == level_max) { // if all food on the map consumed , level up

                ResetGame();
                GameCompleted();

            }
        }
    }
    void ResetGame() {//gets the game ready for another round in the same run
        score = 0;
        ResetPositions(); 
    }
    void ResetPositions() { //OOP at its peak (you'll know when you see the updates of ghosts)
        pacman -> resetPosition();
        for (int i = 0; i < ghostCount; i++) {
            ghosts[i]->ResetPosition();
        }
        
    }
    
    // Helpers

    
	void GameCompleted();
    void ShowWinScreen();
}; 