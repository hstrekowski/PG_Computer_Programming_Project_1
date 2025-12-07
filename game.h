#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

// Definicje stałych rozmiarów okien
#define WINDOW_HEIGHT 40
#define WINDOW_WIDTH 134
#define GAME_SCREEN_HEIGHT 29
#define GAME_SCREEN_WIDTH 132
#define STATUS_AREA_HEIGHT 9
#define STATUS_AREA_WIDTH 132

// Stałe sterowania
#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96

// Limity gry i ustawienia
#define MAX_STARS_LIMIT 200
#define MAX_HUNTERS_LIMIT 50
#define FRAME_RATE 45
#define BASE_SLEEP_TIME 1000000
#define BASE_MOVE_RATE 10
#define TOP_N 5
#define MAX_NICKNAME_LENGTH 30

// Definicje par kolorów
#define PAIR_WHITE 1
#define PAIR_ORANGE 2
#define PAIR_RED 3
#define PAIR_HUNTER_GREEN 4
#define PAIR_HUNTER_CYAN 5
#define PAIR_HUNTER_MAGENTA 6
#define PAIR_HUNTER_BLUE 7

// Znaki Animacji Jaskółki
#define SWALLOW_ANIM_UP_FLAP "^"
#define SWALLOW_ANIM_UP_GLIDE "-"
#define SWALLOW_ANIM_DOWN_FLAP "v"
#define SWALLOW_ANIM_DOWN_GLIDE "-"
#define SWALLOW_ANIM_LEFT_FLAP "<"
#define SWALLOW_ANIM_LEFT_GLIDE "{"
#define SWALLOW_ANIM_RIGHT_FLAP ">"
#define SWALLOW_ANIM_RIGHT_GLIDE "}"

// Ustawienia do kalkulacji punktów
#define TIME_MULTIPLIER 10
#define STAR_MULTIPLIER 100
#define LIVE_MULTIPLIER 300
#define LVL_MULTIPLIER 0.1f
// Ustawienia Jaskółki
#define SWALLOW_INIT_LIVES 3
#define SWALLOW_INIT_SPEED 1

// Znaki Gwiazd
#define STAR_CHAR_NORMAL "*"
#define STAR_CHAR_AGING "+"

// Ustawienia Safe Zone
#define ZONE_CHAR_ACTIVE "O"
#define ZONE_CHAR_INACTIVE " "
#define BLINK_SLEEP_US 60000
#define BLINK_REPEAT_COUNT 3
#define ZONE_COOLDOWN 30
#define ZONE_START_COOLDOWN 5
#define ZONE_RADIUS 1
#define ZONE_ANIMATION_STEPS 8
#define ZONE_ANIMATION_DELAY 10000

// Ustawienia Huntera
#define HUNTER_TYPES_MAX 5
#define HUNTER_DASH_SPEED 1.2f
#define HUNTER_DASH_TOLERANCE 0.99
#define HUNTER_BASE_DASH_WAIT_TIMER 22

// Główne struktury danych gry
typedef struct Stats
{
    int score;
    int starsFumbled;
} Stats;

typedef struct PlayerConfig
{
    char name[MAX_NICKNAME_LENGTH];
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
    int allowedHunterTypes[HUNTER_TYPES_MAX];
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
    char name[MAX_NICKNAME_LENGTH];
    int score;
    int level;
} ScoreEntry;

struct Swallow;

// Deklaracje funkcji głównych
void refresh_windows(WINDOW *windows[], int n);
void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, struct Swallow *swallow, PlayerConfig *config);

#endif