#ifndef HUNTER_H
#define HUNTER_H

#include "game.h"
#include "swallow.h"

typedef struct Hunter
{
    float x; // Pozycja dokładna (float)
    float y;
    float dx;         // Wektor ruchu X
    float dy;         // Wektor ruchu Y
    int width;        // Szerokość (1, 2 lub 3)
    int height;       // Wysokość (1, 2 lub 3)
    int bounces_left; // Liczba odbić (1-3)
    int is_active;
} Hunter;

void init_hunters(Hunter hunters[]);
void try_spawn_hunter(Hunter hunters[], Swallow *swallow, int frame_counter);
void update_hunters(WINDOW *gameScreen, Hunter hunters[], Swallow *swallow);

#endif