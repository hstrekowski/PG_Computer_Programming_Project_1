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

void draw_leaderboard(WINDOW *win, ScoreEntry top[], int count, char *current_player, int current_score)
{
    int cx = GAME_SCREEN_WIDTH / 2;
    int cy = GAME_SCREEN_HEIGHT / 2;

    mvwprintw(win, cy + 2, cx - 10, "--- TOP %d SCORES ---", TOP_N);
    for (int i = 0; i < count; i++)
    {
        // Podświetl aktualnego gracza jeśli to jego wynik
        if (strcmp(top[i].name, current_player) == 0 && top[i].score == current_score)
        {
            wattron(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
        }
        mvwprintw(win, cy + 4 + i, cx - 15, "%d. %-15s LVL %d : %d", i + 1, top[i].name, top[i].level, top[i].score);
        if (strcmp(top[i].name, current_player) == 0 && top[i].score == current_score)
        {
            wattroff(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
        }
    }
}

void draw_game_over(WINDOW *win, PlayerConfig *p, int final_score, int won, ScoreEntry top[], int count, int quit)
{
    wclear(win);
    box(win, 0, 0);

    char *res;
    if (won)
        res = "LEVEL COMPLETED!";
    else if (quit)
        res = "GAME ABORTED (Q)";
    else
        res = "GAME OVER";

    int cx = GAME_SCREEN_WIDTH / 2;
    int cy = GAME_SCREEN_HEIGHT / 2;

    wattron(win, won ? COLOR_PAIR(PAIR_HUNTER_GREEN) : COLOR_PAIR(PAIR_RED));
    mvwprintw(win, cy - 4, cx - (strlen(res) / 2), "%s", res);
    wattroff(win, won ? COLOR_PAIR(PAIR_HUNTER_GREEN) : COLOR_PAIR(PAIR_RED));

    // Rysujemy wynik i tabelę TYLKO jeśli wygrał
    if (won)
    {
        char score_msg[50];
        sprintf(score_msg, "FINAL SCORE: %d", final_score);
        mvwprintw(win, cy - 2, cx - (strlen(score_msg) / 2), "%s", score_msg);
        draw_leaderboard(win, top, count, p->name, final_score);
    }
    else
    {
        // Komunikat dla przegranych
        char *fail_msg = "Mission Failed. No score recorded.";
        mvwprintw(win, cy, cx - (strlen(fail_msg) / 2), "%s", fail_msg);
    }

    wrefresh(win);
}