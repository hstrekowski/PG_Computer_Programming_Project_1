#include "star.h"
#include <stdlib.h>

// Resetowanie tablicy gwiazdek
void init_stars(Star stars[])
{
    for (int i = 0; i < MAX_STARS_LIMIT; i++)
    {
        stars[i].is_active = 0;
        stars[i].sign = STAR_CHAR_NORMAL;
        stars[i].x = 0;
        stars[i].y = 0;
        stars[i].move_timer = 0;
        stars[i].speed_delay = 0;
        stars[i].anim_ticker = 0;
    }
}

// Logika tworzenia nowej gwiazdy
void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter, int spawn_freq, int max_stars)
{
    if (frame_counter % spawn_freq == 0)
    {
        if (*star_index_to_spawn < max_stars && *star_index_to_spawn < MAX_STARS_LIMIT)
        {
            Star *current_star = &stars[*star_index_to_spawn];
            current_star->is_active = 1;
            current_star->x = (rand() % (GAME_SCREEN_WIDTH - 2)) + 1;
            current_star->y = 1;
            current_star->anim_ticker = 0;

            int random_speed = (rand() % 14) + 2;
            current_star->speed_delay = random_speed;
            current_star->move_timer = random_speed;
            (*star_index_to_spawn)++;
        }
    }
}

// Rysowanie animowanej gwiazdy
void draw_star_animated(WINDOW *win, Star *star)
{
    star->anim_ticker++;
    int speed_factor = 20 - (int)(star->y / 1.5);
    if (speed_factor < 2)
        speed_factor = 2;

    int age_color = PAIR_WHITE;
    int third = GAME_SCREEN_HEIGHT / 3;
    if (star->y > third * 2)
        age_color = PAIR_RED;
    else if (star->y > third)
        age_color = PAIR_ORANGE;

    int current_color;
    if ((star->anim_ticker / speed_factor) % 2 == 0)
    {
        star->sign = STAR_CHAR_NORMAL;
        current_color = PAIR_WHITE;
    }
    else
    {
        star->sign = STAR_CHAR_AGING;
        current_color = age_color;
    }

    wattron(win, COLOR_PAIR(current_color));
    mvwprintw(win, star->y, star->x, "%s", star->sign);
    wattroff(win, COLOR_PAIR(current_color));
}

// Aktualizacja pozycji i kolizji gwiazd
void update_stars(WINDOW *gameScreen, Star stars[], Swallow *swallow, Stats *stats)
{
    for (int i = 0; i < MAX_STARS_LIMIT; i++)
    {
        Star *star = &stars[i];
        if (star->is_active)
        {
            if (star->x == swallow->x && star->y == swallow->y)
            {
                mvwprintw(gameScreen, star->y, star->x, " ");
                stats->score++;
                star->is_active = 0;
            }
            else if (star->move_timer <= 0)
            {
                int prev_y = star->y;
                star->y++;
                if (prev_y < GAME_SCREEN_HEIGHT - 1)
                    mvwprintw(gameScreen, prev_y, star->x, " ");
                star->move_timer = star->speed_delay;
            }
            else
            {
                star->move_timer--;
            }

            if (star->y >= GAME_SCREEN_HEIGHT - 1)
            {
                stats->starsFumbled++;
                star->is_active = 0;
            }
            else if (star->is_active)
            {
                draw_star_animated(gameScreen, star);
            }
        }
    }
}