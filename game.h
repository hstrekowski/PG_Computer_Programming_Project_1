#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

// --- STAŁE ---
#define WINDOW_HEIGHT 40
#define WINDOW_WIDTH 134
#define GAME_SCREEN_HEIGHT 29
#define GAME_SCREEN_WIDTH 132
#define STATUS_AREA_HEIGHT 9
#define STATUS_AREA_WIDTH 132

#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96
#define UP_SIGN "^"
#define DOWN_SIGN "v"
#define LEFT_SIGN "<"
#define RIGHT_SIGN ">"

#define MAX_STARS_LIMIT 200
#define MAX_HUNTERS_LIMIT 50
#define FRAME_RATE 45
#define BASE_MOVE_RATE 10

#define PAIR_WHITE 1
#define PAIR_ORANGE 2
#define PAIR_RED 3
#define PAIR_HUNTER_GREEN 4
#define PAIR_HUNTER_CYAN 5
#define PAIR_HUNTER_MAGENTA 6
#define PAIR_HUNTER_BLUE 7

#define PAIR_GOAL_REACHED 8

// --- STRUKTURY ---

typedef struct Stats
{
    int score;
    int starsFumbled;
} Stats;

typedef struct PlayerConfig
{
    char name[30];
    int startLevel;
} PlayerConfig;

// WAŻNE: Ta struktura musi być zdefiniowana PRZED funkcją update_status
typedef struct LevelConfig
{
    int levelNumber;
    int durationSeconds;
    int maxStars;
    int maxHunters;
    int starFreq;
    int hunterFreq;
    int hunterDamage;
    int minSpeed;
    int maxSpeed;
    int allowedHunterTypes[5];
    int starGoal;
} LevelConfig;

typedef struct SafeZone
{
    int is_active;        // 1 = włączona, 0 = wyłączona
    int x, y;             // Środek strefy (pozycja startowa jaskółki)
    int duration_timer;   // Ile czasu strefa jest widoczna (np. 5 sekund)
    int cooldown_timer;   // Odliczanie do ponownego użycia (30 sekund)
    int game_start_timer; // Licznik od początku gry (do blokady 5s)
} SafeZone;

// Forward declaration dla Swallow (bo swallow.h dołącza game.h)
struct Swallow;

// --- PROTOTYPY FUNKCJI ---

void refresh_windows(WINDOW *windows[], int n);

// WAŻNE: Tutaj musi być 'LevelConfig *lvlConfig'
void update_status(WINDOW *statusArea, PlayerConfig *config, LevelConfig *lvlConfig, int score, int lifeForce, int seconds, int speed, int starsFumbled);

void show_start_screen(WINDOW *gameScreen, PlayerConfig *config);
int load_level_config(int level, LevelConfig *lvlConfig);
void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, struct Swallow *swallow, PlayerConfig *config);

#endif