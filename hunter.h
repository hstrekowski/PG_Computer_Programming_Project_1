#ifndef HUNTER_H
#define HUNTER_H

#include "game.h"
#include "swallow.h"

typedef struct Hunter
{
    float x, y, dx, dy;
    int width, height;
    int bounces_left;
    int is_active;
    int color_pair;
    int has_dashed;
    int dash_wait_timer;
} Hunter;

// Zerowanie tablicy hunterów
void init_hunters(Hunter hunters[]);

// Próba zespawnowania huntera (jeśli czas i limity pozwalają)
void try_spawn_hunter(Hunter hunters[], Swallow *s, int fc, int tf, int freq, int max, int allowed[5]);

// Aktualizacja pozycji i logiki wszystkich hunterów
void update_hunters(WINDOW *win, Hunter hunters[], Swallow *s, int dmg, SafeZone *sz);

#endif