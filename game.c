#include "game.h"
#include "swallow.h"
#include "star.h"
#include "hunter.h"
#include "config.h"
#include "replay.h"
#include "safe_zone.h"
#include "highscore.h"
#include "render.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct GameState
{
    LevelConfig lvl;
    Stats stats;
    SafeZone sz;
    Star stars[MAX_STARS_LIMIT];
    Hunter hunters[MAX_HUNTERS_LIMIT];
    int frames;
    int total_frames;
    int fc;
    int move_ctr;
    int star_idx;
} GameState;

void refresh_windows(WINDOW *windows[], int n)
{
    for (int i = 0; i < n; i++)
        wrefresh(windows[i]);
}

int init_game_state(GameState *g, Swallow *s, PlayerConfig *p, WINDOW *win)
{
    if (!load_level_config(p->startLevel, &g->lvl))
    {
        wclear(win);
        box(win, 0, 0);

        char msg[60];
        sprintf(msg, "ERROR: Missing level%d.txt", p->startLevel);

        int cy = GAME_SCREEN_HEIGHT / 2;
        int cx = (GAME_SCREEN_WIDTH - strlen(msg)) / 2;

        wattron(win, COLOR_PAIR(PAIR_RED));
        mvwprintw(win, cy, cx, "%s", msg);
        mvwprintw(win, cy + 2, (GAME_SCREEN_WIDTH - 20) / 2, "Press any key...");
        wattroff(win, COLOR_PAIR(PAIR_RED));

        wrefresh(win);

        nodelay(win, FALSE);
        wgetch(win);

        return 0;
    }

    s->minSpeedLimit = g->lvl.minSpeed;
    s->maxSpeedLimit = g->lvl.maxSpeed;
    if (s->speed < s->minSpeedLimit)
        s->speed = s->minSpeedLimit;
    if (s->speed > s->maxSpeedLimit)
        s->speed = s->maxSpeedLimit;

    g->stats.score = 0;
    g->stats.starsFumbled = 0;
    g->frames = g->lvl.durationSeconds * FRAME_RATE;
    g->total_frames = g->frames;
    g->fc = 0;
    g->move_ctr = BASE_MOVE_RATE;
    g->star_idx = 0;

    init_safe_zone(&g->sz);
    init_stars(g->stars);
    init_hunters(g->hunters);
    srand(time(NULL));

    wattron(win, COLOR_PAIR(PAIR_WHITE));
    mvwprintw(win, s->y, s->x, "%s", s->sign);
    wattroff(win, COLOR_PAIR(PAIR_WHITE));
    wrefresh(win);
    return 1;
}

int process_input(WINDOW *win, Swallow *s, GameState *g)
{
    int ch = wgetch(win);

    // Obsługa Safe Zone (T)
    handle_safe_zone_input(&g->sz, s, ch, win);

    // Jeśli to nie był klawisz 'T' i nie jest to błąd odczytu
    if (ch != ERR && ch != 't' && ch != 'T')
    {

        if (!g->sz.is_active)
        {
            // ZMIANA: Przekazujemy 'ch' bezpośrednio, bez ungetch()
            if (handle_input(s, ch))
                return 1; // Zwróć 1 jeśli wciśnięto Q
        }
        else
        {
            // W safe zone działa tylko Q
            if (ch == 'q' || ch == 'Q')
                return 1;
        }
    }
    return 0;
}

void process_logic(GameState *g, Swallow *s, WINDOW *win)
{
    if (g->sz.is_active)
    {
        s->x = g->sz.x;
        s->y = g->sz.y;
        wattron(win, COLOR_PAIR(PAIR_ORANGE));
        mvwprintw(win, s->y, s->x, "%s", s->sign);
        wattroff(win, COLOR_PAIR(PAIR_ORANGE));
    }
    else
    {
        update_swallow_position(win, s, &g->move_ctr);
    }

    try_spawn_star(g->stars, &g->star_idx, g->fc, g->lvl.starFreq, g->lvl.maxStars);
    update_stars(win, g->stars, s, &g->stats);

    try_spawn_hunter(g->hunters, s, g->fc, g->total_frames, g->lvl.hunterFreq, g->lvl.maxHunters, g->lvl.allowedHunterTypes);
    update_hunters(win, g->hunters, s, g->lvl.hunterDamage, &g->sz);

    update_safe_zone(&g->sz);
}

void process_render(WINDOW *win, WINDOW *stat, GameState *g, Swallow *s, PlayerConfig *p)
{
    box(win, 0, 0);
    draw_safe_zone(win, &g->sz);
    draw_status(stat, p, &g->lvl, &g->stats, s->lifeForce, g->frames / FRAME_RATE, s->speed, &g->sz);
    wrefresh(win);
}

void handle_game_over(WINDOW *win, WINDOW *statArea, GameState *g, Swallow *s, PlayerConfig *p, ReplaySystem *replay, int quit)
{
    int won = (s->lifeForce > 0 && g->stats.score >= g->lvl.starGoal && !quit);
    int final_score = 0;
    ScoreEntry top[TOP_N];
    int count = 0;

    if (won)
    {
        final_score = calculate_final_score(&g->stats, s->lifeForce, g->frames, g->lvl.levelNumber);
        save_score(p->name, final_score, g->lvl.levelNumber);
        count = load_top_scores(top, TOP_N);
    }

    // Rysujemy ekran raz
    draw_game_over(win, p, final_score, won, top, count, quit);

    nodelay(win, FALSE);
    while (1)
    {
        int ch = wgetch(win);
        if (ch == 'q' || ch == 'Q')
        {
            break;
        }
        if (ch == 'r' || ch == 'R')
        {
            play_replay(replay, win, statArea, p, &g->lvl);

            // Po powrocie z powtórki odświeżamy ekran końcowy
            draw_game_over(win, p, final_score, won, top, count, quit);
        }
    }
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow, PlayerConfig *config)
{
    GameState g;
    if (!init_game_state(&g, swallow, config, gameScreen))
        return;

    // NOWE: Inicjalizacja replayu
    ReplaySystem replay;
    init_replay(&replay);

    const int SLEEP = 1000000 / FRAME_RATE;
    draw_status(statusArea, config, &g.lvl, &g.stats, swallow->lifeForce, g.frames / FRAME_RATE, swallow->speed, &g.sz);

    int user_quit = 0;

    while (g.frames > 0 && swallow->lifeForce > 0)
    {
        if (process_input(gameScreen, swallow, &g))
        {
            user_quit = 1;
            break;
        }

        process_logic(&g, swallow, gameScreen);

        // NOWE: Nagrywamy klatkę (zanim sprawdzimy win condition, żeby mieć ostatni moment)
        record_frame(&replay, swallow, g.stars, g.hunters, &g.sz, &g.stats, swallow->lifeForce, g.frames);

        if (g.stats.score >= g.lvl.starGoal)
        {
            process_render(gameScreen, statusArea, &g, swallow, config);
            usleep(500000);
            break;
        }

        process_render(gameScreen, statusArea, &g, swallow, config);
        usleep(SLEEP);
        g.frames--;
        g.fc++;
        g.move_ctr--;
    }

    // Przekazujemy replay system do game over
    handle_game_over(gameScreen, statusArea, &g, swallow, config, &replay, user_quit);

    // Sprzątamy pamięć po replayu
    free_replay(&replay);
}