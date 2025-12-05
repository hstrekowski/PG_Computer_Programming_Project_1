#include "game.h"
#include "swallow.h"
#include "star.h"
#include "hunter.h"
#include "config.h"
#include "safe_zone.h"
#include "render.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void refresh_windows(WINDOW *windows[], int n)
{
    for (int i = 0; i < n; i++)
        wrefresh(windows[i]);
}

typedef struct
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

int init_game_state(GameState *g, Swallow *s, PlayerConfig *p, WINDOW *win)
{
    if (!load_level_config(p->startLevel, &g->lvl))
    {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 14, 10, "ERROR: Missing level%d.txt", p->startLevel);
        wrefresh(win);
        return 0;
    }

    // --- LOGIKA APPLY LIMITS ---
    s->minSpeedLimit = g->lvl.minSpeed;
    s->maxSpeedLimit = g->lvl.maxSpeed;
    if (s->speed < s->minSpeedLimit)
        s->speed = s->minSpeedLimit;
    if (s->speed > s->maxSpeedLimit)
        s->speed = s->maxSpeedLimit;
    // ---------------------------

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
    handle_safe_zone_input(&g->sz, s, ch, win);

    if (ch != ERR && ch != 't' && ch != 'T')
    {
        ungetch(ch);
        if (!g->sz.is_active)
        {
            if (handle_input(win, s))
                return 1;
        }
        else
        {
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
    draw_safe_zone(win, &g->sz);
    draw_status(stat, p, &g->lvl, &g->stats, s->lifeForce, g->frames / FRAME_RATE, s->speed, &g->sz);
    wrefresh(win);
}

void handle_game_over(WINDOW *win, GameState *g, Swallow *s, PlayerConfig *p)
{
    int won = (s->lifeForce > 0 && g->stats.score >= g->lvl.starGoal);
    draw_game_over(win, p, &g->stats, s->lifeForce, g->lvl.starGoal, won);
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow, PlayerConfig *config)
{
    GameState g;
    if (!init_game_state(&g, swallow, config, gameScreen))
        return;

    const int SLEEP = 1000000 / FRAME_RATE;

    draw_status(statusArea, config, &g.lvl, &g.stats, swallow->lifeForce, g.frames / FRAME_RATE, swallow->speed, &g.sz);

    while (g.frames > 0 && swallow->lifeForce > 0)
    {
        if (process_input(gameScreen, swallow, &g))
            break;
        process_logic(&g, swallow, gameScreen);
        process_render(gameScreen, statusArea, &g, swallow, config);
        usleep(SLEEP);
        g.frames--;
        g.fc++;
        g.move_ctr--;
    }

    handle_game_over(gameScreen, &g, swallow, config);
}