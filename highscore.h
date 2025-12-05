#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "game.h"

int calculate_final_score(Stats *s, int lives, int frames_left, int level_num);
void save_score(char *name, int score, int level);
// Wczytuje do tablicy 'entries' max 'max_count' wyników. Zwraca ilość wczytanych.
int load_top_scores(ScoreEntry entries[], int max_count);

#endif