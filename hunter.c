#include "hunter.h"
#include <stdlib.h>
#include <math.h>

// Resetowanie tablicy przeciwników
void init_hunters(Hunter hunters[])
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        hunters[i].is_active = 0;
        hunters[i].has_dashed = 0;
        hunters[i].dash_wait_timer = 0;
    }
}

// Obliczanie dynamicznego limitu wrogów
int get_dynamic_limit(int max, int current_frame, int total)
{
    int rem = total - current_frame;
    int limit = max;
    if (rem <= total / 4)
        limit += 2;
    else if (rem <= total / 2)
        limit += 1;
    return (limit > MAX_HUNTERS_LIMIT) ? MAX_HUNTERS_LIMIT : limit;
}

// Losowanie właściwości nowego wroga
void setup_hunter_props(Hunter *h, int *allowed)
{
    h->is_active = 1;
    h->has_dashed = 0;
    h->dash_wait_timer = 0;
    int type;
    do
    {
        type = rand() % 5;
    } while (allowed[type] == 0);

    int w[] = {1, 2, 1, 3, 2};
    int he[] = {2, 1, 3, 1, 2};
    int cols[] = {PAIR_HUNTER_GREEN, PAIR_HUNTER_CYAN, PAIR_HUNTER_MAGENTA, PAIR_HUNTER_BLUE, PAIR_RED};
    h->width = w[type];
    h->height = he[type];
    h->color_pair = cols[type];
}

// Obliczanie wektora ruchu wroga
void spawn_vectors(Hunter *h, Swallow *s)
{
    float tx = (float)s->x, ty = (float)s->y;
    float dx = tx - h->x, dy = ty - h->y;
    float len = sqrt(dx * dx + dy * dy);
    float spd = 0.5f;
    if (len != 0)
    {
        h->dx = (dx / len) * spd;
        h->dy = (dy / len) * spd;
    }
    else
    {
        h->dx = spd;
        h->dy = spd;
    }
}

// Logika spawnowania wroga
void try_spawn_hunter(Hunter hunters[], Swallow *s, int fc, int tf, int freq, int max, int allowed[5])
{
    if (fc % freq != 0)
        return;
    int limit = get_dynamic_limit(max, fc, tf);
    int act = 0, slot = -1;
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        if (hunters[i].is_active)
            act++;
        else if (slot == -1)
            slot = i;
    }
    if (act >= limit || slot == -1)
        return;
    Hunter *h = &hunters[slot];
    setup_hunter_props(h, allowed);
    int rem = tf - fc;
    int min_b = (rem <= tf / 4) ? 3 : ((rem <= tf / 2) ? 2 : 1);
    h->bounces_left = (rand() % 3) + min_b;
    int c = rand() % 4;
    h->x = (c % 2 == 0) ? 2 : GAME_SCREEN_WIDTH - 3;
    h->y = (c < 2) ? 2 : GAME_SCREEN_HEIGHT - 3;
    spawn_vectors(h, s);
}

// Uruchomienie ataku (szarży)
void execute_dash_launch(Hunter *h, Swallow *s)
{
    float tx = s->x - h->x;
    float ty = s->y - h->y;
    float dist = sqrt(tx * tx + ty * ty);
    float dash_speed = HUNTER_DASH_SPEED;
    if (dist > 0)
    {
        h->dx = (tx / dist) * dash_speed;
        h->dy = (ty / dist) * dash_speed;
    }
    h->has_dashed = 1;
}

// Logika odbijania od ścian
void check_bounce_logic(Hunter *h, Swallow *s, int hx, int hy)
{
    if (h->has_dashed)
    {
        if (hx)
            h->dx = -h->dx;
        if (hy)
            h->dy = -h->dy;
        return;
    }
    float pdx = hx ? -h->dx : h->dx;
    float pdy = hy ? -h->dy : h->dy;
    float tx = s->x - h->x, ty = s->y - h->y;
    float dist = sqrt(tx * tx + ty * ty);
    int need_dash = 0;
    if (dist > 0)
    {
        float plen = sqrt(pdx * pdx + pdy * pdy);
        float dot = (pdx / plen * tx / dist) + (pdy / plen * ty / dist);
        if (dot < 0.99)
            need_dash = 1;
    }
    if (need_dash)
        h->dash_wait_timer = 22;
    else
    {
        h->dx = pdx;
        h->dy = pdy;
    }
}

// Rysowanie wroga na ekranie
void draw_hunter(WINDOW *win, Hunter *h)
{
    wattron(win, COLOR_PAIR(h->color_pair));
    for (int ry = 0; ry < h->height; ry++)
        for (int rx = 0; rx < h->width; rx++)
            mvwprintw(win, (int)h->y + ry, (int)h->x + rx, "%d", h->bounces_left);
    wattroff(win, COLOR_PAIR(h->color_pair));
}

// Sprawdzanie kolizji z obiektami
static int check_hunter_collisions(Hunter *h, Swallow *s, SafeZone *sz, int dmg)
{
    // Safe Zone
    if (sz->is_active)
    {
        int zr = 1;
        int zl = sz->x - zr, zr_ = sz->x + zr;
        int zt = sz->y - zr, zb = sz->y + zr;
        if ((int)h->x <= zr_ && (int)h->x + h->width >= zl &&
            (int)h->y <= zb && (int)h->y + h->height >= zt)
        {
            h->is_active = 0;
            return 1;
        }
    }
    // Player
    int hit = (s->x >= (int)h->x && s->x < (int)h->x + h->width &&
               s->y >= (int)h->y && s->y < (int)h->y + h->height);
    if (hit && !sz->is_active)
    {
        if (s->lifeForce > 0)
            s->lifeForce -= dmg;
        h->is_active = 0;
        return 1;
    }
    return 0;
}

// Aktualizacja pojedynczego wroga
void update_single_hunter(WINDOW *win, Hunter *h, Swallow *s, int dmg, SafeZone *sz)
{
    if (h->dash_wait_timer > 0)
    {
        h->dash_wait_timer--;
        if (h->dash_wait_timer == 0)
            execute_dash_launch(h, s);
        draw_hunter(win, h);
        return;
    }

    for (int ry = 0; ry < h->height; ry++)
        for (int rx = 0; rx < h->width; rx++)
            mvwprintw(win, (int)h->y + ry, (int)h->x + rx, " ");

    h->x += h->dx;
    h->y += h->dy;

    int hx = (h->x <= 1) || (h->x + h->width >= GAME_SCREEN_WIDTH - 1);
    int hy = (h->y <= 1) || (h->y + h->height >= GAME_SCREEN_HEIGHT - 1);
    if (hx)
        h->x = (h->x <= 1) ? 1.1f : GAME_SCREEN_WIDTH - 1 - h->width - 0.1f;
    if (hy)
        h->y = (h->y <= 1) ? 1.1f : GAME_SCREEN_HEIGHT - 1 - h->height - 0.1f;

    if (hx || hy)
    {
        check_bounce_logic(h, s, hx, hy);
        if (--h->bounces_left == 0)
        {
            h->is_active = 0;
            return;
        }
    }

    if (check_hunter_collisions(h, s, sz, dmg))
        return;

    draw_hunter(win, h);
}

// Pętla aktualizacji wszystkich wrogów
void update_hunters(WINDOW *win, Hunter hunters[], Swallow *s, int dmg, SafeZone *sz)
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        if (hunters[i].is_active)
            update_single_hunter(win, &hunters[i], s, dmg, sz);
    }
}