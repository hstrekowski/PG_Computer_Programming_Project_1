#include "safe_zone.h"
#include <stdlib.h>
#include <unistd.h>

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

// Efekt wizualny mrugania tłem
void blink_effect(WINDOW *win)
{
    for (int i = 0; i < BLINK_REPEAT_COUNT; i++)
    {
        wbkgd(win, COLOR_PAIR(PAIR_ORANGE) | A_REVERSE);
        wrefresh(win);
        usleep(BLINK_SLEEP_US);
        wbkgd(win, COLOR_PAIR(0));
        wrefresh(win);
        usleep(BLINK_SLEEP_US);
    }
}

// Aktywacja logiki bezpiecznej strefy
void activate_zone(SafeZone *sz, Swallow *swallow, WINDOW *win)
{
    if (sz->game_start_timer > 5 * FRAME_RATE && sz->cooldown_timer <= 0)
    {
        blink_effect(win);

        sz->is_active = 1;
        sz->duration_timer = 5 * FRAME_RATE;
        sz->cooldown_timer = 30 * FRAME_RATE;
        mvwprintw(win, swallow->y, swallow->x, " ");

        swallow->x = sz->x;
        swallow->y = sz->y;
        swallow->direction = UP;
        swallow->sign = SWALLOW_ANIM_UP_FLAP;
    }
}

// Sprawdzenie czy wciśnięto przycisk strefy
void handle_safe_zone_input(SafeZone *sz, Swallow *swallow, int ch, WINDOW *win)
{
    if (ch == 't' || ch == 'T')
        activate_zone(sz, swallow, win);
}

// Aktualizacja timerów odliczających czas
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

// Rysowanie obszaru bezpiecznej strefy
void draw_safe_zone(WINDOW *win, SafeZone *sz)
{
    if (!sz->is_active && sz->duration_timer < 0)
        return;
    int radius = 1;
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