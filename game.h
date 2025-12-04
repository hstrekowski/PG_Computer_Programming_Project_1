#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

// Screen constants
#define WINDOW_HEIGHT 40
#define WINDOW_WIDTH 134

#define GAME_SCREEN_HEIGHT 29
#define GAME_SCREEN_WIDTH 132

#define STATUS_AREA_HEIGHT 9
#define STATUS_AREA_WIDTH 132

// Direction constants
#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96

// Swallow sign constants
#define UP_SIGN "^"
#define DOWN_SIGN "v"
#define LEFT_SIGN "<"
#define RIGHT_SIGN ">"

// Game logic constants
#define MAX_STARS 100
#define FRAME_RATE 45
#define DURATION_SECONDS 60
#define BASE_MOVE_RATE 10
#define MAX_HUNTERS 10

typedef struct Stats
{
    int score;
    int starsFumbled;
} Stats;

// Pomocnicze funkcje UI
void refresh_windows(WINDOW *windows[], int n);
void update_status(WINDOW *statusArea, int score, int lifeForce, int seconds, int speed, int starsFumbled);

struct Swallow;

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, struct Swallow *swallow);

#endif