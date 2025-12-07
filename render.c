#include "render.h"
#include <string.h>

// Rysowanie nagłówka statusu
void draw_status_header(WINDOW *win, PlayerConfig *p, LevelConfig *l)
{
    mvwprintw(win, 1, 2, "PLAYER: %s", p->name);
    mvwprintw(win, 1, 40, "LEVEL: %d", l->levelNumber);
}

// Rysowanie statystyk i żyć
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

// Rysowanie statusu Safe Zone
void draw_zone_status(WINDOW *win, SafeZone *sz)
{
    if (sz->is_active)
    {
        wattron(win, COLOR_PAIR(PAIR_HUNTER_BLUE));
        mvwprintw(win, 6, 55, "ZONE: ACTIVE %ds ", (sz->duration_timer / FRAME_RATE) + 1);
        wattroff(win, COLOR_PAIR(PAIR_HUNTER_BLUE));
    }
    else if (sz->cooldown_timer > 0)
    {
        mvwprintw(win, 6, 55, "ZONE: WAIT %ds  ", (sz->cooldown_timer / FRAME_RATE) + 1);
    }
    else if (sz->game_start_timer > ZONE_START_COOLDOWN * FRAME_RATE)
    {
        wattron(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
        mvwprintw(win, 6, 55, "ZONE: READY!    ");
        wattroff(win, COLOR_PAIR(PAIR_HUNTER_GREEN));
    }
    else
    {
        mvwprintw(win, 6, 55, "ZONE: LOCKED    ");
    }
}

// Główna funkcja rysowania statusu
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
    draw_zone_status(win, sz);
    mvwprintw(win, 6, 20, "O/P - Speed");
    wattron(win, COLOR_PAIR(PAIR_RED));
    mvwprintw(win, 6, 90, "Q - QUIT");
    wattroff(win, COLOR_PAIR(PAIR_RED));
    wrefresh(win);
}

// Wyświetlanie tabeli wyników
void draw_leaderboard(WINDOW *win, ScoreEntry top[], int count, char *curr_pl, int curr_sc, int cy, int cx)
{
    char *hdr = "--- HALL OF FAME ---";
    mvwprintw(win, cy, cx - (strlen(hdr) / 2), "%s", hdr);

    for (int i = 0; i < count; i++)
    {
        char entry[100];
        sprintf(entry, "%d. %-15s %6d (LVL %d)", i + 1, top[i].name, top[i].score, top[i].level);
        int is_me = (strcmp(top[i].name, curr_pl) == 0 && top[i].score == curr_sc);
        if (is_me)
            wattron(win, COLOR_PAIR(PAIR_HUNTER_GREEN) | A_BOLD);
        mvwprintw(win, cy + 2 + i, cx - (strlen(entry) / 2), "%s", entry);
        if (is_me)
            wattroff(win, COLOR_PAIR(PAIR_HUNTER_GREEN) | A_BOLD);
    }
}

// Rysowanie ekranu końca gry
void draw_game_over(WINDOW *win, PlayerConfig *p, int final_score, int won, ScoreEntry top[], int count, int quit)
{
    wclear(win);
    box(win, 0, 0);
    int cx = GAME_SCREEN_WIDTH / 2;
    int cy = GAME_SCREEN_HEIGHT / 2;

    char *title = won ? "=== MISSION ACCOMPLISHED ===" : "=== MISSION FAILED ===";
    if (quit)
        title = "=== ABORTED ===";

    int title_color = won ? PAIR_HUNTER_GREEN : PAIR_RED;
    wattron(win, COLOR_PAIR(title_color) | A_BOLD);
    mvwprintw(win, cy - 8, cx - (strlen(title) / 2), "%s", title);
    wattroff(win, COLOR_PAIR(title_color) | A_BOLD);

    if (won)
    {
        mvwprintw(win, cy - 5, cx - 15, "PLAYER: %s", p->name);
        mvwprintw(win, cy - 4, cx - 15, "SCORE:  %d", final_score);
        draw_leaderboard(win, top, count, p->name, final_score, cy - 1, cx);
    }
    else
    {
        char *reason = quit ? "You gave up." : "The swallow has fallen.";
        mvwprintw(win, cy - 2, cx - (strlen(reason) / 2), "%s", reason);
    }

    char *footer = "[ R - WATCH REPLAY ]    [ Q - EXIT GAME ]";
    mvwprintw(win, GAME_SCREEN_HEIGHT - 3, cx - (strlen(footer) / 2), "%s", footer);
    wrefresh(win);
}