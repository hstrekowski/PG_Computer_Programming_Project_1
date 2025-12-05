#include "render.h"
#include <string.h>

void draw_status_header(WINDOW *win, PlayerConfig *p, LevelConfig *l)
{
    mvwprintw(win, 1, 2, "PLAYER: %s", p->name);
    mvwprintw(win, 1, 40, "LEVEL: %d", l->levelNumber);
}

void draw_status_stats(WINDOW *win, LevelConfig *l, Stats *s, int lives, int time, int speed)
{
    if (s->score >= l->starGoal)
    {
        wattron(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
        mvwprintw(win, 2, 2, "STARS: %d / %d (OK)", s->score, l->starGoal);
        wattroff(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
    }
    else
    {
        mvwprintw(win, 2, 2, "STARS: %d / %d", s->score, l->starGoal);
    }

    int color = (lives <= 1 ? PAIR_RED : (lives == 2 ? PAIR_ORANGE : PAIR_WHITE));
    wattron(win, COLOR_PAIR(color));
    mvwprintw(win, 2, 40, "LIVES: %d", lives);
    wattroff(win, COLOR_PAIR(color));

    mvwprintw(win, 2, 70, "TIME: %d s", time);
    mvwprintw(win, 3, 2, "FUMBLED: %d", s->starsFumbled);
    mvwprintw(win, 3, 40, "SPEED: %d", speed);
}

void draw_zone_status(WINDOW *win, SafeZone *sz)
{
    if (sz->is_active)
    {
        wattron(win, COLOR_PAIR(PAIR_HUNTER_BLUE));
        mvwprintw(win, 5, 85, "ZONE: ACTIVE %ds ", (sz->duration_timer / FRAME_RATE) + 1);
        wattroff(win, COLOR_PAIR(PAIR_HUNTER_BLUE));
    }
    else if (sz->cooldown_timer > 0)
    {
        mvwprintw(win, 5, 85, "ZONE: WAIT %ds  ", (sz->cooldown_timer / FRAME_RATE) + 1);
    }
    else if (sz->game_start_timer > 5 * FRAME_RATE)
    {
        wattron(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
        mvwprintw(win, 5, 85, "ZONE: READY!    ");
        wattroff(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
    }
    else
    {
        mvwprintw(win, 5, 85, "ZONE: LOCKED    ");
    }
}

void draw_status(WINDOW *win, PlayerConfig *p, LevelConfig *l, Stats *s, int lives, int time, int speed, SafeZone *sz)
{
    werase(win);
    box(win, 0, 0);
    draw_status_header(win, p, l);
    draw_status_stats(win, l, s, lives, time, speed);
    mvwhline(win, 4, 1, 0, STATUS_AREA_WIDTH - 2);
    mvwprintw(win, 5, 2, "[ CONTROLS ]");
    mvwprintw(win, 5, 20, "W/S/A/D - Move");
    mvwprintw(win, 5, 55, "T - SAFE ZONE");
    mvwprintw(win, 6, 20, "O/P - Speed");
    wattron(win, COLOR_PAIR(PAIR_RED));
    mvwprintw(win, 6, 90, "Q - QUIT");
    wattroff(win, COLOR_PAIR(PAIR_RED));
    draw_zone_status(win, sz);
    wrefresh(win);
}

void draw_game_over(WINDOW *win, PlayerConfig *p, Stats *s, int lives, int goal, int won)
{
    wclear(win);
    box(win, 0, 0);
    char *res = won ? "LEVEL COMPLETED!" : (lives <= 0 ? "GAME OVER - Died!" : "GAME OVER - Time!");
    char stats[150];
    sprintf(stats, "Player: %s | Stars: %d/%d | Lives: %d", p->name, s->score, goal, lives < 0 ? 0 : lives);

    int cy = GAME_SCREEN_HEIGHT / 2;
    int cx = GAME_SCREEN_WIDTH / 2;
    mvwprintw(win, cy - 2, cx - (strlen(res) / 2), "%s", res);
    mvwprintw(win, cy, cx - (strlen(stats) / 2), "%s", stats);
    wrefresh(win);
}