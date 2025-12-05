#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

// Stałe ekranu
#define WINDOW_HEIGHT 40
#define WINDOW_WIDTH 134
#define GAME_SCREEN_HEIGHT 29
#define GAME_SCREEN_WIDTH 132
#define STATUS_AREA_HEIGHT 9
#define STATUS_AREA_WIDTH 132

// Stałe kierunków i znaków
#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96
#define UP_SIGN "^"
#define DOWN_SIGN "v"
#define LEFT_SIGN "<"
#define RIGHT_SIGN ">"

// Limity i ustawienia
#define MAX_STARS_LIMIT 200
#define MAX_HUNTERS_LIMIT 50
#define FRAME_RATE 45
#define BASE_MOVE_RATE 10
#define TOP_N 5 //

// Kolory
#define PAIR_WHITE 1
#define PAIR_ORANGE 2
#define PAIR_RED 3
#define PAIR_HUNTER_GREEN 4
#define PAIR_HUNTER_CYAN 5
#define PAIR_HUNTER_MAGENTA 6
#define PAIR_HUNTER_BLUE 7

// Struktury
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
    int is_active;
    int x, y;
    int duration_timer;
    int cooldown_timer;
    int game_start_timer;
} SafeZone;

typedef struct ScoreEntry
{
    char name[30];
    int score;
    int level;
} ScoreEntry;

// Forward declaration
struct Swallow;

// Funkcje ogólne
void refresh_windows(WINDOW *windows[], int n);
void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, struct Swallow *swallow, PlayerConfig *config);

#endif