# 🦅 Swallow Stars

> **A high-performance, retro arcade survival game running entirely in the terminal.**
>
> Built with **C** and **Ncurses**. Featuring vector-based physics, a custom replay system, and modular architecture.

![Swallow Stars Banner](path/to/your/banner_image.png)
---

## 📖 About The Project

**Swallow Stars** to zaawansowany projekt gry zręcznościowej napisany w czystym języku C. Gra łączy klasyczną mechanikę "unikania i zbierania" z nowoczesnymi rozwiązaniami architektonicznymi.

Projekt został stworzony z myślą o **Czystym Kodzie (Clean Code)**. Nie znajdziesz tu "magicznych liczb" ani gigantycznych funkcji. Kod jest ściśle zmodularyzowany, oddzielając logikę biznesową od warstwy prezentacji (renderingu) oraz zarządzania pamięcią.

### Key Highlights:
* **Instant Replay System:** Cały stan gry jest nagrywany do RAM w każdej klatce. Po przegranej możesz obejrzeć dokładną powtórkę swojej rozgrywki.
* **Tactical Safe Zone:** Mechanika "Blink & Reset" pozwalająca na strategiczne wycofanie się z trudnej sytuacji.
* **Physics-Based Enemies:** Przeciwnicy używają wektorów do poruszania się i odbijania od ścian.

---

## 📸 Gallery

| Main Menu | Gameplay Action |
|:---:|:---:|
| ![Menu Screen](path/to/menu_screenshot.png) | ![Gameplay Screen](path/to/gameplay_screenshot.png) |
| *Konfigurowalny start gry* | *Walka o przetrwanie* |

| Replay Mode | Game Over & Stats |
|:---:|:---:|
| ![Replay Screen](path/to/replay_screenshot.png) | ![Scoreboard Screen](path/to/scoreboard_screenshot.png) |
| *Odtwarzanie powtórki* | *Hall of Fame* |

---

## 🚀 Features & Mechanics

### 🌟 Aging Stars
Gwiazdy to nie tylko punkty. Zmieniają one swoje zachowanie i wygląd w zależności od pozycji na ekranie:
* **Normal (White):** Standardowy stan.
* **Warning (Orange):** Gwiazda zbliża się do dołu ekranu.
* **Critical (Red):** Gwiazda zaraz zniknie, co liczy się jako błąd (Fumble).

### 🛡️ Safe Zone (The "Blink")
Pod klawiszem `T` kryje się potężna umiejętność. Po naładowaniu paska (stan "READY"):
1.  Gra wykonuje szybki efekt wizualny (Screen Blink).
2.  Wszyscy wrogowie w pobliżu środka są usuwani.
3.  Gracz jest teleportowany do bezpiecznej pozycji.

### 📼 Memory-Based Replay
Gra nie odtwarza wideo, lecz **re-symuluje** stan gry.
* Wykorzystuje dynamiczną alokację pamięci (`malloc`) do przechowywania tysięcy klatek (`ReplayFrame`).
* Podczas powtórki silnik podmienia aktywny stan gry na ten z bufora pamięci.

---

## 📂 Codebase Architecture

Struktura projektu jest płaska i modularna. Każdy plik odpowiada za konkretny aspekt domeny:

* **`main.c`** – Entry point. Inicjalizacja biblioteki Ncurses i kolorów.
* **`game.c`** – Główna pętla gry (Game Loop), zarządzanie czasem ramki (FPS) i stanem (`GameState`).
* **`swallow.c`** – Logika gracza. Sterowanie, animacja ASCII, zarządzanie życiem.
* **`hunter.c`** – AI przeciwników. Obliczanie wektorów `dx/dy`, kolizje, logika szarży (Dash).
* **`star.c`** – Logika znajdziek. Spawnowanie, animacja mrugania.
* **`safe_zone.c`** – Logika strefy bezpiecznej. Timery, cooldowny, efekt wizualny.
* **`replay.c`** – System powtórek. Zarządzanie buforem klatek w pamięci sterty.
* **`render.c`** – Warstwa widoku. Rysowanie UI, HUD, tabeli wyników.
* **`highscore.c`** – Obsługa plików. Zapis/Odczyt `highscores.txt`, sortowanie wyników.
* **`config.c`** – Parser plików tekstowych. Wczytywanie poziomów z `levelX.txt`.
* **`game.h`** – Centralny nagłówek. Definicje stałych (NO MAGIC NUMBERS), struktury i prototypy.

---

## 🛠️ Build & Installation

### Prerequisites
Wymagany jest kompilator GCC oraz biblioteka `ncurses`.

* **Debian/Ubuntu:** `sudo apt-get install libncurses5-dev libncursesw5-dev`
* **Fedora:** `sudo dnf install ncurses-devel`
* **MacOS:** `brew install ncurses`

### Compilation
Użyj poniższego polecenia w katalogu głównym projektu:

```bash
gcc -o swallow_stars main.c game.c config.c highscore.c hunter.c render.c replay.c safe_zone.c star.c swallow.c -lncurses -lm