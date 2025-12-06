#include "replay.h"
#include "render.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Alokacja pamięci dla powtórki
void init_replay(ReplaySystem *r)
{
    r->frames = (ReplayFrame *)malloc(sizeof(ReplayFrame) * MAX_REPLAY_FRAMES);
    r->frame_count = 0;
    r->max_frames = MAX_REPLAY_FRAMES;
}

// Zwalnianie pamięci powtórki
void free_replay(ReplaySystem *r)
{
    if (r->frames)
    {
        free(r->frames);
        r->frames = NULL;
    }
}

// Zapis stanu do klatki
void record_frame(ReplaySystem *r, Swallow *s, Star stars[], Hunter hunters[], SafeZone *sz, Stats *st, int lives, int f_left)
{
    if (r->frame_count >= r->max_frames)
        return;
    ReplayFrame *f = &r->frames[r->frame_count];
    f->swallow = *s;
    f->safeZone = *sz;
    f->stats = *st;
    f->lifeForce = lives;
    f->frames_left = f_left;
    memcpy(f->stars, stars, sizeof(Star) * MAX_STARS_LIMIT);
    memcpy(f->hunters, hunters, sizeof(Hunter) * MAX_HUNTERS_LIMIT);
    r->frame_count++;
}

// Pomocnicze rysowanie gwiazdy w replayu
static void draw_replay_star(WINDOW *win, Star *s)
{
    int speed_factor = 20 - (int)(s->y / 1.5);
    if (speed_factor < 2)
        speed_factor = 2;

    int age_color = PAIR_WHITE;
    if (s->y > (GAME_SCREEN_HEIGHT / 3) * 2)
        age_color = PAIR_RED;
    else if (s->y > GAME_SCREEN_HEIGHT / 3)
        age_color = PAIR_ORANGE;

    int current_color;
    char *sign;

    if ((s->anim_ticker / speed_factor) % 2 == 0)
    {
        sign = STAR_CHAR_NORMAL;
        current_color = PAIR_WHITE;
    }
    else
    {
        sign = STAR_CHAR_AGING;
        current_color = age_color;
    }

    wattron(win, COLOR_PAIR(current_color));
    mvwprintw(win, s->y, s->x, "%s", sign);
    wattroff(win, COLOR_PAIR(current_color));
}

// Rysowanie całej sceny z klatki
static void render_replay_scene(WINDOW *win, ReplayFrame *f)
{
    wclear(win);
    box(win, 0, 0);
    draw_safe_zone(win, &f->safeZone);

    int color = PAIR_WHITE;
    if (f->safeZone.is_active || f->safeZone.duration_timer > 0)
        color = PAIR_ORANGE;
    else if (f->lifeForce == 2)
        color = PAIR_ORANGE;
    else if (f->lifeForce <= 1)
        color = PAIR_RED;

    wattron(win, COLOR_PAIR(color));
    mvwprintw(win, f->swallow.y, f->swallow.x, "%s", f->swallow.sign);
    wattroff(win, COLOR_PAIR(color));

    for (int j = 0; j < MAX_STARS_LIMIT; j++)
    {
        if (f->stars[j].is_active)
            draw_replay_star(win, &f->stars[j]);
    }

    for (int j = 0; j < MAX_HUNTERS_LIMIT; j++)
    {
        if (f->hunters[j].is_active)
        {
            Hunter *h = &f->hunters[j];
            wattron(win, COLOR_PAIR(h->color_pair));
            for (int ry = 0; ry < h->height; ry++)
                for (int rx = 0; rx < h->width; rx++)
                    mvwprintw(win, (int)h->y + ry, (int)h->x + rx, "%d", h->bounces_left);
            wattroff(win, COLOR_PAIR(h->color_pair));
        }
    }
    wrefresh(win);
}

// Odtwarzanie nagranej rozgrywki
void play_replay(ReplaySystem *r, WINDOW *gameWin, WINDOW *statWin, PlayerConfig *p, LevelConfig *l)
{
    const int SLEEP = 1000000 / FRAME_RATE;
    nodelay(gameWin, TRUE);
    int was_active = 0;

    for (int i = 0; i < r->frame_count; i++)
    {
        if (wgetch(gameWin) == 'q' || wgetch(gameWin) == 'Q')
            break;
        ReplayFrame *f = &r->frames[i];

        if (f->safeZone.is_active && !was_active)
            blink_effect(gameWin);
        was_active = f->safeZone.is_active;

        render_replay_scene(gameWin, f);

        draw_status(statWin, p, l, &f->stats, f->lifeForce, f->frames_left / FRAME_RATE, f->swallow.speed, &f->safeZone);
        wattron(statWin, COLOR_PAIR(PAIR_HUNTER_MAGENTA));
        mvwprintw(statWin, 5, 2, "[ REPLAY MODE ]");
        mvwprintw(statWin, 6, 90, "Q - STOP REPLAY");
        wattroff(statWin, COLOR_PAIR(PAIR_HUNTER_MAGENTA));
        wrefresh(statWin);
        usleep(SLEEP);
    }
}