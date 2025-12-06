#ifndef STAR_H
#define STAR_H

#include "game.h"
#include "swallow.h"

// Struktura gwiazdki
typedef struct Star
{
    int x;
    int y;
    char *sign;
    int is_active;
    int move_timer;
    int speed_delay;
    int anim_ticker;
} Star;

// Inicjalizacja tablicy gwiazd
void init_stars(Star stars[]);

// Próba spawnowania nowej gwiazdy
void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter, int spawn_freq, int max_stars);

// Aktualizacja i rysowanie gwiazd
void update_stars(WINDOW *gameScreen, Star stars[], Swallow *swallow, Stats *stats);

#endif