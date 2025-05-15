Pac-Man Game 

A modern C++ implementation of the classic Pac-Man arcade game using SFML graphics library. 

Table of Contents 

Overview 

Features 

Game Elements 

Ghost Types 

Prerequisites 

Installation 

Controls 

Gameplay 

File Structure 

Creating Custom Levels 

Troubleshooting 

Credits 

Overview 

This game is a recreation of the classic Pac-Man arcade game with additional features including multiple ghost types with unique abilities, super mode for Pac-Man, and a scoring system with persistent high scores. 

Features 

Main menu system with Start, Instructions, and Exit options 

Interactive countdown before game start 

Multiple unique ghost types with different behaviors 

Super mode when eating power pellets 

Score tracking and high score system 

Lives system with visual indicators 

Game over screen with customized messages based on performance 

Background music and sound effects 

Game Elements 

Pac-Man: The player character who must eat all the dots while avoiding ghosts 

Regular Dots: Worth 10 points each 

Super Food/Power Pellets: Worth 50 points, activates super mode 

Ghosts: Enemies that chase Pac-Man; each ghost has unique abilities 

Maze: The environment with walls and pathways 

Ghost Types 

The game features several ghost types, each with unique abilities: 

Ghost Type 

Ability Description 

TELEPORTER 

Can teleport around the maze randomly 

RANDOM 

Moves in unpredictable patterns 

RAGE 

Has bursts of high speed 

AMBUSHER 

Camps near super food to ambush Pac-Man 

HERMES 

Moves very quickly throughout the maze 

PHANTOM 

Has an extended hit radius 

TIME STOP 

Can temporarily freeze Pac-Man 

RING 

Can become invisible periodically 

Prerequisites 

C++ compiler with C++11 support or higher 

SFML 2.5.0 or higher 

CMake 3.10 or higher (optional, for building) 

Installation 

Option 1: Using CMake (recommended)  

Clone the repository or download the source code https://github.com/azan-wasty/PacmanGame 

Install SFML libraries for your system 

Create a build directory and navigate to it: mkdir build && cd build 
 

Run CMake: cmake .. 
 

Build the project: make 
 

Run the game: ./pacman 
 

Option 2: Manual Compilation 

Ensure SFML is installed on your system 

Compile the main file with all dependencies: g++ -o pacman main.cpp Ghost.cpp Maze.cpp Pacman.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 

Sfml 2.5.1 used 
 

Run the game: ./pacman 
 

Controls 

Menu Navigation: 

Up/Down Arrow Keys: Navigate menu items 

Enter: Select menu item 

Game Controls: 

Arrow Keys: Move Pac-Man up, down, left, right 

Enter: Return to menu (after game over) 

Gameplay 

Start the game from the main menu 

Navigate Pac-Man through the maze using arrow keys 

Eat all dots while avoiding ghosts 

Collect super food to enter super mode and be able to eat ghosts 

When all dots are eaten, you win the game 

If a ghost catches you, you lose a life 

Game ends when all lives are lost 

File Structure 

├── main.cpp               # Main game logic 

├── Entity.h                                               # Main game logic 
  
├── Pacman.h               # Pac-Man character implementation 
├── Ghost.h                # Ghost base class and implementations 
├── Maze.h                 # Maze implementation 

├── Animation.h             # Sprite Animation implementation 

 
├── sounds/                # Game sound effects 
│   ├── chomp.wav          # Dot eating sound 
│   ├── death.wav          # Death sound 
│   ├── super.wav          # Super mode sound 
│   └── menu.wav           # Menu music 
├── sprites/               # Game sprites 
│   ├── PACMANLEFT.png     # Pac-Man facing left 
│   ├── PACMANRIGHT.png    # Pac-Man facing right 
│   ├── PACMANUP.png       # Pac-Man facing up 
│   ├── PACMANDOWN.png     # Pac-Man facing down 
│   └── “GHOSTNAME”.png    # Ghost sprites 
└── ArcadeClassic.ttf      # Game font 
  

Creating Custom Levels 

The maze is defined in the Maze.cpp file. To create custom levels: 

Modify the maze array in the Maze constructor 

Use these characters to define the maze:  

#: Wall 

.: Regular dot 

o: Super food 

P: Pac-Man starting position 

0-7: Ghost starting positions 

(space): Empty walkable space 

Troubleshooting 

Common Issues 

Missing Font Error: Make sure "ArcadeClassic.ttf" is in the same directory as the executable 

Error: Could not load font ArcadeClassic.ttf 
  

Solution: Download and place the ArcadeClassic.ttf font in the game directory 

Missing Sprite Files: Ensure all sprite files are in the correct locations Solution: Check the sprites directory and make sure all required PNG files exist 

SFML Library Not Found: If you get linker errors Solution: Make sure SFML is properly installed and linked in your build system 

Credits 

Azan Wasty (24L-2557) 

Rana Nauman Iqbal (24L-2565) 

Dawood Majeed (24L-2538) 

BDS-2A 
