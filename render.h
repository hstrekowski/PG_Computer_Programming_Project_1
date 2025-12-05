#ifndef RENDER_H
#define RENDER_H

#include "game.h"
#include "safe_zone.h"

// Rysuje dolny pasek statusu
void draw_status(WINDOW *win, PlayerConfig *p, LevelConfig *l, Stats *s, int lives, int time, int speed, SafeZone *sz);

// Rysuje ekran końcowy (Game Over / Win)
void draw_game_over(WINDOW *win, PlayerConfig *p, int final_score, int won, ScoreEntry top[], int count, int quit);

#endif