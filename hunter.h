#ifndef HUNTER_H
#define HUNTER_H

#include "game.h"
#include "swallow.h"
#include "safe_zone.h"

// Struktura przeciwnika (Hunter)
typedef struct Hunter
{
    float x, y;
    float dx, dy;
    int width, height;
    int color_pair;
    int bounces_left;
    int is_active;
    int has_dashed;
    int dash_wait_timer;
} Hunter;

// Inicjalizacja tablicy hunterów
void init_hunters(Hunter hunters[]);

// Próba zespawnowania nowego huntera
void try_spawn_hunter(Hunter hunters[], Swallow *s, int fc, int tf, int freq, int max, int allowed[5]);

// Aktualizacja wszystkich hunterów
void update_hunters(WINDOW *win, Hunter hunters[], Swallow *s, int dmg, SafeZone *sz);

#endif