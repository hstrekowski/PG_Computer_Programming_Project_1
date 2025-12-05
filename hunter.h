#ifndef HUNTER_H
#define HUNTER_H

#include "game.h"
#include "swallow.h"

typedef struct Hunter
{
    float x;
    float y;
    float dx;
    float dy;
    int width;
    int height;
    int bounces_left;
    int is_active;
    int color_pair;
    int has_dashed;
} Hunter;

void init_hunters(Hunter hunters[]);
void try_spawn_hunter(Hunter hunters[], Swallow *swallow, int frame_counter, int total_frames, int spawn_freq, int max_hunters, int allowed_types[5]);

// ZMIANA: Dodano damage
void update_hunters(WINDOW *gameScreen, Hunter hunters[], Swallow *swallow, int damage, SafeZone *safeZone);

#endif