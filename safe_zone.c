#include "safe_zone.h"
#include <stdlib.h>

void init_safe_zone(SafeZone *sz)
{
    sz->is_active = 0;
    sz->x = GAME_SCREEN_WIDTH / 2;
    sz->y = GAME_SCREEN_HEIGHT - 6;
    sz->duration_timer = 0;
    sz->cooldown_timer = 0;
    sz->game_start_timer = 0;
}

void activate_zone(SafeZone *sz, Swallow *swallow, WINDOW *win)
{
    if (sz->game_start_timer > 5 * FRAME_RATE && sz->cooldown_timer <= 0)
    {
        sz->is_active = 1;
        sz->duration_timer = 5 * FRAME_RATE;
        sz->cooldown_timer = 30 * FRAME_RATE;

        mvwprintw(win, swallow->y, swallow->x, " ");

        swallow->x = sz->x;
        swallow->y = sz->y;

        // TERAZ UŻYWAMY STAŁYCH Z GAME.H
        swallow->direction = UP;
        swallow->sign = UP_SIGN;
    }
}

void handle_safe_zone_input(SafeZone *sz, Swallow *swallow, int ch, WINDOW *win)
{
    if (ch == 't' || ch == 'T')
    {
        activate_zone(sz, swallow, win);
    }
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
        {
            sz->is_active = 0;
        }
    }
}

void draw_safe_zone(WINDOW *win, SafeZone *sz)
{
    // Jeśli nieaktywna I timer jest poniżej 0 (-1), to znaczy że już posprzątane. Wychodzimy.
    if (!sz->is_active && sz->duration_timer < 0)
        return;

    int radius = 1;

    // Jeśli aktywna -> rysuj "O". Jeśli nieaktywna (ale timer == 0) -> rysuj spacje " "
    char *pixel = sz->is_active ? "O" : " ";
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

    // Jeśli właśnie wyczyściliśmy (czyli byliśmy tu, gdy active=0 i timer=0),
    // ustawiamy timer na -1, żeby w kolejnej klatce funkcja od razu wyszła (return na początku).
    if (!sz->is_active)
    {
        sz->duration_timer = -1;
    }
}