#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_defaults(LevelConfig *cfg, int level)
{
    cfg->levelNumber = level;
    cfg->durationSeconds = 60;
    cfg->maxStars = 50;
    cfg->maxHunters = 5;
    cfg->starFreq = 15;
    cfg->hunterFreq = 100;
    cfg->hunterDamage = 1;
    cfg->minSpeed = 1;
    cfg->maxSpeed = 5;
    cfg->starGoal = 10;
    for (int i = 0; i < 5; i++)
        cfg->allowedHunterTypes[i] = 1;
}

void parse_line(FILE *f, LevelConfig *cfg)
{
    char key[50];
    if (fscanf(f, "%s", key) == EOF)
        return;

    if (strcmp(key, "DURATION") == 0)
        fscanf(f, "%d", &cfg->durationSeconds);
    else if (strcmp(key, "MAX_STARS") == 0)
        fscanf(f, "%d", &cfg->maxStars);
    else if (strcmp(key, "MAX_HUNTERS") == 0)
        fscanf(f, "%d", &cfg->maxHunters);
    else if (strcmp(key, "STAR_FREQ") == 0)
        fscanf(f, "%d", &cfg->starFreq);
    else if (strcmp(key, "HUNTER_FREQ") == 0)
        fscanf(f, "%d", &cfg->hunterFreq);
    else if (strcmp(key, "HUNTER_DAMAGE") == 0)
        fscanf(f, "%d", &cfg->hunterDamage);
    else if (strcmp(key, "STAR_GOAL") == 0)
        fscanf(f, "%d", &cfg->starGoal);
    else if (strcmp(key, "SPEED_LIMITS") == 0)
        fscanf(f, "%d %d", &cfg->minSpeed, &cfg->maxSpeed);
    else if (strcmp(key, "HUNTER_TYPES") == 0)
    {
        for (int i = 0; i < 5; i++)
            fscanf(f, "%d", &cfg->allowedHunterTypes[i]);
    }
}

int load_level_config(int level, LevelConfig *lvlConfig)
{
    char filename[50];
    sprintf(filename, "level%d.txt", level);
    FILE *file = fopen(filename, "r");
    if (!file)
        return 0;

    set_defaults(lvlConfig, level);
    while (!feof(file))
    {
        parse_line(file, lvlConfig);
    }
    fclose(file);
    return 1;
}

void show_start_screen(WINDOW *gameScreen, PlayerConfig *config)
{
    echo();
    curs_set(1);
    int box_h = 10, box_w = 50;
    int sy = (GAME_SCREEN_HEIGHT - box_h) / 2;
    int sx = (GAME_SCREEN_WIDTH - box_w) / 2;

    WINDOW *win = derwin(gameScreen, box_h, box_w, sy, sx);
    box(win, 0, 0);
    mvwprintw(win, 0, (box_w - 12) / 2, " GAME SETUP ");

    mvwprintw(win, 3, 4, "Enter Nickname:");
    wrefresh(win);
    mvwgetnstr(win, 4, 4, config->name, 29);

    mvwprintw(win, 6, 4, "Level (1-10):");
    wrefresh(win);
    char buf[10];
    mvwgetnstr(win, 7, 4, buf, 9);
    config->startLevel = atoi(buf);

    delwin(win);
    wclear(gameScreen);
    box(gameScreen, 0, 0);
    wrefresh(gameScreen);
    noecho();
    curs_set(0);
}