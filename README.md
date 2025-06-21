Your README is well-structured and content-rich — great job laying everything out in detail. Here's a **refined version** that improves grammar, clarity, formatting, and professional tone while keeping your original structure intact:

---

# Pac-Man Game

A modern C++ implementation of the classic Pac-Man arcade game using the SFML graphics library.

---

## 📌 Table of Contents

* [Overview](#overview)
* [Features](#features)
* [Game Elements](#game-elements)
* [Ghost Types](#ghost-types)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
* [Controls](#controls)
* [Gameplay](#gameplay)
* [File Structure](#file-structure)
* [Creating Custom Levels](#creating-custom-levels)
* [Troubleshooting](#troubleshooting)
* [Credits](#credits)

---

## 🕹️ Overview

This is a recreation of the classic Pac-Man arcade game, enhanced with additional features such as multiple ghost types with unique abilities, a super mode for Pac-Man, and a scoring system with persistent high scores.

---

## 🎯 Features

* Main menu with **Start**, **Instructions**, and **Exit** options
* Interactive countdown before game starts
* Multiple ghost types with distinct behaviors
* Super mode activated by power pellets
* Score tracking with high score persistence
* Life system with visual indicators
* Game Over screen with performance-based messages
* Background music and sound effects

---

## 🧩 Game Elements

| Element           | Description                                     |
| ----------------- | ----------------------------------------------- |
| **Pac-Man**       | Player character, collects dots & avoids ghosts |
| **Regular Dots**  | Worth 10 points each                            |
| **Power Pellets** | Worth 50 points; activates super mode           |
| **Ghosts**        | Enemies with unique chasing behavior            |
| **Maze**          | Game environment of walls and paths             |

---

## 👻 Ghost Types

| Ghost Type     | Ability Description                 |
| -------------- | ----------------------------------- |
| **TELEPORTER** | Randomly teleports across the maze  |
| **RANDOM**     | Moves unpredictably                 |
| **RAGE**       | Occasionally bursts into high speed |
| **AMBUSHER**   | Camps near power pellets            |
| **HERMES**     | Moves very quickly                  |
| **PHANTOM**    | Has an extended hit radius          |
| **TIME STOP**  | Can temporarily freeze Pac-Man      |
| **RING**       | Periodically becomes invisible      |

---

## ⚙️ Prerequisites

* C++ compiler with **C++11** support or higher
* **SFML 2.5.0** or higher
* **CMake 3.10** or higher (optional, but recommended for building)

---

## 🔧 Installation

### Option 1: Using CMake (Recommended)

1. Clone the repository:
   `git clone https://github.com/azan-wasty/PacmanGame`
2. Install SFML for your OS.
3. Create and navigate to the build directory:
   `mkdir build && cd build`
4. Run CMake:
   `cmake ..`
5. Build the project:
   `make`
6. Run the game:
   `./pacman`

### Option 2: Manual Compilation

1. Ensure SFML is installed and linked.
2. Compile:

   ```
   g++ -o pacman main.cpp Ghost.cpp Maze.cpp Pacman.cpp \
   -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
   ```

   *(SFML version 2.5.1 used)*
3. Run the game:
   `./pacman`

---

## 🎮 Controls

### Menu Navigation

* **Up/Down Arrows**: Navigate menu
* **Enter**: Select option

### In-Game

* **Arrow Keys**: Move Pac-Man
* **Enter**: Return to menu (after Game Over)

---

## 🧠 Gameplay

1. Start the game from the main menu.
2. Navigate Pac-Man through the maze using arrow keys.
3. Collect all dots while avoiding ghosts.
4. Eat power pellets to activate super mode and eat ghosts.
5. Win the game by clearing all dots.
6. Lose a life if caught by a ghost. Game ends after all lives are lost.

---

## 🗂️ File Structure

```
├── main.cpp              # Main game logic
├── Entity.h              # Abstract base class
├── Pacman.h              # Pac-Man character
├── Ghost.h               # Ghost behaviors
├── Maze.h                # Maze design and logic
├── Animation.h           # Sprite animation handling

├── sounds/               # Sound effects
│   ├── chomp.wav
│   ├── death.wav
│   ├── super.wav
│   └── menu.wav

├── sprites/              # Game sprite assets
│   ├── PACMANLEFT.png
│   ├── PACMANRIGHT.png
│   ├── PACMANUP.png
│   ├── PACMANDOWN.png
│   └── <GHOSTNAME>.png

└── ArcadeClassic.ttf     # Game font
```

---

## ✏️ Creating Custom Levels

Custom levels are defined in `Maze.cpp`. To modify:

* Edit the character array in the `Maze` constructor
* Use the following character codes:

  * `#`: Wall
  * `.`: Regular dot
  * `o`: Power pellet
  * `P`: Pac-Man start position
  * `0-7`: Ghost spawn positions
  * (space): Walkable space

---

## 🛠️ Troubleshooting

| Issue                  | Solution                                                  |
| ---------------------- | --------------------------------------------------------- |
| **Missing Font**       | Ensure `ArcadeClassic.ttf` is in the executable directory |
| **Missing Sprites**    | Verify sprite files exist in the `sprites/` folder        |
| **SFML Linker Errors** | Confirm SFML is correctly installed and linked            |

---

## 🙌 Credits

Developed by:

* **Azan Wasty** (24L-2557)
* **Rana Nauman Iqbal** (24L-2565)
* **Dawood Majeed** (24L-2538)

**FAST-NUCES Lahore | BDS-2A**

---

Let me know if you'd like this turned into a proper [`README.md`](f) file you can paste directly into your GitHub repo.
