#ifndef STAR_H
#define STAR_H

#include "game.h"
#include "swallow.h"

typedef struct Star
{
    int x;
    int y;
    char *sign;
    int is_active;
    int move_timer;
} Star;

void init_stars(Star stars[]);
void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter);
void update_stars(WINDOW *gameScreen, Star stars[], Swallow *swallow, Stats *stats);

#endif