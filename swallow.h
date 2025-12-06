#ifndef SWALLOW_H
#define SWALLOW_H

#include "game.h"

// Struktura postaci gracza
typedef struct Swallow
{
    int x;
    int y;
    int speed;
    int direction;
    int lifeForce;
    char *sign;
    int minSpeedLimit;
    int maxSpeedLimit;
    int anim_ticker;
} Swallow;

// Inicjalizacja gracza
void init_swallow(Swallow *swallow);

// Obsługa klawiszy sterowania
int handle_input(Swallow *swallow, int ch);

// Aktualizacja pozycji gracza
void update_swallow_position(WINDOW *gameScreen, Swallow *swallow, int *move_counter);

#endif