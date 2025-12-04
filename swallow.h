#ifndef SWALLOW_H
#define SWALLOW_H

#include "game.h"

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
} Swallow;

void init_swallow(Swallow *swallow);
int handle_input(WINDOW *gameScreen, Swallow *swallow);
void update_swallow_position(WINDOW *gameScreen, Swallow *swallow, int *move_counter);

#endif