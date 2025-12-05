#include "highscore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Formuła: (Gwiazdki*100 + Życia*300 + Czas*10) * (1 + 0.1 * Level)
int calculate_final_score(Stats *s, int lives, int frames_left, int level_num)
{
    int time_bonus = (frames_left / FRAME_RATE) * 10;
    int base_score = (s->score * 100) + (lives * 300) + time_bonus;

    float multiplier = 1.0f + (level_num * 0.1f);
    return (int)(base_score * multiplier);
}

void save_score(char *name, int score, int level)
{
    FILE *f = fopen("highscores.txt", "a");
    if (!f)
        return;
    // Zapisujemy: Nazwa Wynik Poziom
    fprintf(f, "%s %d %d\n", name, score, level);
    fclose(f);
}

int compare_scores(const void *a, const void *b)
{
    ScoreEntry *sa = (ScoreEntry *)a;
    ScoreEntry *sb = (ScoreEntry *)b;
    return sb->score - sa->score; // Malejąco
}

int load_top_scores(ScoreEntry entries[], int max_count)
{
    FILE *f = fopen("highscores.txt", "r");
    if (!f)
        return 0;

    // Tymczasowa tablica na dużo wyników (np. 100), potem posortujemy
    ScoreEntry temp[100];
    int count = 0;

    while (count < 100 && fscanf(f, "%s %d %d", temp[count].name, &temp[count].score, &temp[count].level) == 3)
    {
        count++;
    }
    fclose(f);

    qsort(temp, count, sizeof(ScoreEntry), compare_scores);

    int return_count = (count < max_count) ? count : max_count;
    for (int i = 0; i < return_count; i++)
    {
        entries[i] = temp[i];
    }
    return return_count;
}