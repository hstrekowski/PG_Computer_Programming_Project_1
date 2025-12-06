#include "safe_zone.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Ustawienie wartości początkowych strefy
void init_safe_zone(SafeZone *sz)
{
    sz->is_active = 0;
    sz->x = GAME_SCREEN_WIDTH / 2;
    sz->y = GAME_SCREEN_HEIGHT - 6;
    sz->duration_timer = 0;
    sz->cooldown_timer = 0;
    sz->game_start_timer = 0;
}

// Przy uzyciu safe_zone ekran miga na zolto
void blink_effect(WINDOW *win)
{
    for (int i = 0; i < BLINK_REPEAT_COUNT; i++)
    {
        // Żółte tło
        wbkgd(win, COLOR_PAIR(PAIR_ORANGE) | A_REVERSE);
        wrefresh(win);
        usleep(BLINK_SLEEP_US);

        // Normalne tło
        wbkgd(win, COLOR_PAIR(0));
        wrefresh(win);
        usleep(BLINK_SLEEP_US);
    }
}

// Rysuje lub zmazuje safe_zone
static void draw_flying_box(WINDOW *win, int cx, int cy, int show_player, char *sign, int erase)
{
    int color = PAIR_ORANGE;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int py = cy + dy;
            int px = cx + dx;

            if (py > 0 && py < GAME_SCREEN_HEIGHT - 1 && px > 0 && px < GAME_SCREEN_WIDTH - 1)
            {
                if (erase)
                {
                    mvwprintw(win, py, px, ZONE_CHAR_INACTIVE);
                }
                else
                {
                    // Ramka
                    if (abs(dx) == 1 || abs(dy) == 1)
                    {
                        wattron(win, COLOR_PAIR(color));
                        mvwprintw(win, py, px, ZONE_CHAR_ACTIVE);
                        wattroff(win, COLOR_PAIR(color));
                    }
                    // Środek (Gracz)
                    else if (dx == 0 && dy == 0 && show_player)
                    {
                        wattron(win, COLOR_PAIR(PAIR_WHITE) | A_BOLD);
                        mvwprintw(win, py, px, "%s", sign);
                        wattroff(win, COLOR_PAIR(PAIR_WHITE) | A_BOLD);
                    }
                }
            }
        }
    }
    wrefresh(win);
}

// Animacja transportu z wykorzystaniem blink_effect
static void animate_transport(WINDOW *win, int p_x, int p_y, int z_x, int z_y, char *sign)
{
    int steps = ZONE_ANIMATION_STEPS; // Mniej kroków = szybciej
    int delay = ZONE_ANIMATION_DELAY; // 10ms = bardzo szybko

    // Strefa leci do gracza
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        int cur_x = z_x + (int)((p_x - z_x) * t);
        int cur_y = z_y + (int)((p_y - z_y) * t);

        draw_flying_box(win, cur_x, cur_y, 0, sign, 0);
        usleep(delay);
        draw_flying_box(win, cur_x, cur_y, 0, sign, 1);
    }

    // Moment złapania gracza
    blink_effect(win);

    // Strefa wraca z graczem
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / steps;
        int cur_x = p_x + (int)((z_x - p_x) * t);
        int cur_y = p_y + (int)((z_y - p_y) * t);

        // Wymaż oryginalną pozycję gracza na starcie powrotu
        if (i == 0)
            mvwprintw(win, p_y, p_x, " ");

        draw_flying_box(win, cur_x, cur_y, 1, sign, 0);
        usleep(delay);

        if (i < steps)
            draw_flying_box(win, cur_x, cur_y, 1, sign, 1);
    }
}

void activate_zone(SafeZone *sz, Swallow *swallow, WINDOW *win)
{
    if (sz->game_start_timer > ZONE_START_COOLDOWN * FRAME_RATE && sz->cooldown_timer <= 0)
    {

        // Uruchomienie animacji (która w środku wywoła blink_effect)
        animate_transport(win, swallow->x, swallow->y, sz->x, sz->y, swallow->sign);

        sz->is_active = 1;
        sz->duration_timer = ZONE_START_COOLDOWN * FRAME_RATE;
        sz->cooldown_timer = ZONE_COOLDOWN * FRAME_RATE;
        mvwprintw(win, swallow->y, swallow->x, " ");

        swallow->x = sz->x;
        swallow->y = sz->y;
        swallow->direction = UP;
        swallow->sign = SWALLOW_ANIM_UP_FLAP;
    }
}

void handle_safe_zone_input(SafeZone *sz, Swallow *swallow, int ch, WINDOW *win)
{
    if (ch == 't' || ch == 'T')
        activate_zone(sz, swallow, win);
}

void update_safe_zone(SafeZone *sz)
{
    sz->game_start_timer++;
    if (sz->cooldown_timer > 0)
        sz->cooldown_timer--;
    if (sz->is_active)
    {
        sz->duration_timer--;
        if (sz->duration_timer <= 0)
            sz->is_active = 0;
    }
}

void draw_safe_zone(WINDOW *win, SafeZone *sz)
{
    if (!sz->is_active && sz->duration_timer < 0)
        return;
    int radius = ZONE_RADIUS;
    char *pixel = sz->is_active ? ZONE_CHAR_ACTIVE : ZONE_CHAR_INACTIVE;
    int pair = sz->is_active ? PAIR_ORANGE : 0;

    if (sz->is_active)
        wattron(win, COLOR_PAIR(pair));
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            if (abs(dx) == radius || abs(dy) == radius)
            {
                int d_y = sz->y + dy;
                int d_x = sz->x + dx;
                if (d_y > 0 && d_y < GAME_SCREEN_HEIGHT - 1 && d_x > 0 && d_x < GAME_SCREEN_WIDTH - 1)
                    mvwprintw(win, d_y, d_x, "%s", pixel);
            }
        }
    }
    if (sz->is_active)
        wattroff(win, COLOR_PAIR(pair));
    if (!sz->is_active)
        sz->duration_timer = -1;
}