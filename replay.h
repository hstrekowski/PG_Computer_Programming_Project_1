#ifndef REPLAY_H
#define REPLAY_H

#include "game.h"
#include "swallow.h"
#include "star.h"
#include "hunter.h"
#include "safe_zone.h"

// Limit klatek powtórki
#define MAX_REPLAY_FRAMES 4050

// Struktura pojedynczej klatki nagrania
typedef struct
{
    Swallow swallow;
    Star stars[MAX_STARS_LIMIT];
    Hunter hunters[MAX_HUNTERS_LIMIT];
    SafeZone safeZone;
    Stats stats;
    int lifeForce;
    int frames_left;
} ReplayFrame;

// Struktura systemu powtórek
typedef struct
{
    ReplayFrame *frames;
    int frame_count;
    int max_frames;
} ReplaySystem;

void init_replay(ReplaySystem *r);
void free_replay(ReplaySystem *r);

// Nagrywanie jednej klatki
void record_frame(ReplaySystem *r, Swallow *s, Star stars[], Hunter hunters[], SafeZone *sz, Stats *st, int lives, int f_left);

// Odtwarzanie nagrania
void play_replay(ReplaySystem *r, WINDOW *gameWin, WINDOW *statWin, PlayerConfig *p, LevelConfig *l);

#endif