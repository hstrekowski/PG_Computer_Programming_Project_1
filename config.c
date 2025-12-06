#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ustawienie domyślnych parametrów poziomu
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

// Parsowanie pojedynczej linii pliku
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

// Otwarcie pliku i wczytanie
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

// Pomocnicze rysowanie formularza UI
static void draw_start_form(WINDOW *win, int box_w)
{
    box(win, 0, 0);
    char *title = "SWALLOW STARS";
    wattron(win, A_BOLD);
    mvwprintw(win, 2, (box_w - strlen(title)) / 2, "%s", title);
    wattroff(win, A_BOLD);
    mvwhline(win, 3, 2, 0, box_w - 4);

    char *lbl_nick = "ENTER NICKNAME";
    mvwprintw(win, 6, (box_w - strlen(lbl_nick)) / 2, "%s", lbl_nick);

    char *lbl_lvl = "SELECT LEVEL (1-10)";
    mvwprintw(win, 9, (box_w - strlen(lbl_lvl)) / 2, "%s", lbl_lvl);
    wrefresh(win);
}

// Główna logika ekranu startowego
void show_start_screen(WINDOW *gameScreen, PlayerConfig *config)
{
    echo();
    curs_set(1);

    int box_h = 14, box_w = 60;
    int sy = (GAME_SCREEN_HEIGHT - box_h) / 2;
    int sx = (GAME_SCREEN_WIDTH - box_w) / 2;

    WINDOW *win = derwin(gameScreen, box_h, box_w, sy, sx);
    draw_start_form(win, box_w);

    // Pobieranie Nicku
    int input_start_x = (box_w - 29) / 2;
    mvwgetnstr(win, 7, input_start_x, config->name, 29);

    // Pobieranie Levelu
    char buf[10];
    int lvl_start_x = (box_w - 2) / 2;
    mvwgetnstr(win, 10, lvl_start_x, buf, 9);
    config->startLevel = atoi(buf);

    delwin(win);
    wclear(gameScreen);
    box(gameScreen, 0, 0);
    wrefresh(gameScreen);
    noecho();
    curs_set(0);
}