# 🦅 Swallow Stars

> **A high-performance, retro arcade survival game running entirely in the terminal.**
>
> Built with **C** and **Ncurses**. Featuring vector-based physics, a custom memory-based replay system, and modular clean code architecture.

---

## 📖 About The Project

**Swallow Stars** is an advanced arcade game project developed in pure C. It combines classic "dodge and collect" mechanics with modern software engineering practices.

The project was engineered with a focus on **Clean Code principles**. You won't find "magic numbers" or monolithic functions here. The codebase is strictly modular, separating business logic from the presentation layer (rendering) and memory management.

### Key Highlights:
* **Instant Replay System:** The entire game state is recorded to RAM every frame. Upon defeat, you can watch a frame-by-frame replay of your gameplay.
* **Tactical Safe Zone:** A "Blink & Reset" mechanic allowing for strategic withdrawal from impossible situations.
* **Physics-Based Enemies:** Hunters use vector math for movement and wall bouncing logic.

---


## 🚀 Features & Mechanics

### 🌟 Aging Stars
Stars are dynamic entities, not just static points. They change behavior and appearance based on their vertical position:
* **Normal (White):** Standard state.
* **Warning (Orange):** The star is approaching the danger zone.
* **Critical (Red):** The star is about to vanish (counting as a Fumble).

### 🛡️ Safe Zone (The "Blink")
Mapped to the `T` key, this is a powerful ability. When the gauge reaches "READY":
1.  The game executes a rapid visual **Screen Blink** effect.
2.  All enemies near the center are cleared.
3.  The player is teleported to a safe position.

### 📼 Memory-Based Replay
The game does not record video; it **re-simulates** the game state.
* It utilizes dynamic memory allocation (`malloc`) to store thousands of `ReplayFrame` structures.
* During replay mode, the engine swaps the active game state with data from the memory buffer.

---

## 📂 Codebase Architecture

The project structure is flat and modular. Each file is responsible for a specific domain aspect:

* **`main.c`** – Entry point. Ncurses initialization and color setup.
* **`game.c`** – The Core Game Loop, FPS capping, and `GameState` management.
* **`swallow.c`** – Player logic. Input handling, ASCII animation, life management.
* **`hunter.c`** – Enemy AI. Vector `dx/dy` calculations, collision detection, Dash logic.
* **`star.c`** – Collectible logic. Spawning algorithms and blinking animations.
* **`safe_zone.c`** – Safe Zone logic. Timers, cooldowns, and visual effects.
* **`replay.c`** – Replay System. Heap memory buffer management.
* **`render.c`** – View layer. Drawing the UI, HUD, and Scoreboard (decoupled from logic).
* **`highscore.c`** – File I/O. Reading/Writing `highscores.txt`, sorting results.
* **`config.c`** – Text Parser. Loading level parameters from `levelX.txt`.
* **`game.h`** – Central Header. Constant definitions (**NO MAGIC NUMBERS**), structs, and prototypes.

---

## 🛠️ Build & Installation

### Prerequisites
You need a GCC compiler and the `ncurses` library installed.

* **Debian/Ubuntu:** `sudo apt-get install libncurses5-dev libncursesw5-dev`
* **Fedora:** `sudo dnf install ncurses-devel`
* **MacOS:** `brew install ncurses`

### Compilation
Run the following command in the project root directory:

```bash
gcc -o swallow_stars main.c game.c config.c highscore.c hunter.c render.c replay.c safe_zone.c star.c swallow.c -lncurses -lm