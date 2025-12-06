#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "game.h"

// Obliczanie finalnego wyniku punktowego
int calculate_final_score(Stats *s, int lives, int frames_left, int level_num);

// Zapisanie wyniku do pliku
void save_score(char *name, int score, int level);

// Wczytanie najlepszych wyników
int load_top_scores(ScoreEntry entries[], int max_count);

#endif