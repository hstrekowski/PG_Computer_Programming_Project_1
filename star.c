#include "star.h"
#include <stdlib.h>

void init_stars(Star stars[])
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        stars[i].is_active = 0;
        stars[i].sign = "*";
        stars[i].x = 0;
        stars[i].y = 0;
        stars[i].move_timer = 0;
        stars[i].speed_delay = 0;
    }
}

void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter)
{
    const int STAR_SPAWN_FREQUENCY_FRAMES = 15;

    if (frame_counter % STAR_SPAWN_FREQUENCY_FRAMES == 0 && *star_index_to_spawn < MAX_STARS)
    {
        Star *current_star = &stars[*star_index_to_spawn];
        current_star->is_active = 1;

        // Losowanie pozycji X
        current_star->x = (rand() % (GAME_SCREEN_WIDTH - 2)) + 1;
        current_star->y = 1;

        // LOSOWANIE PRĘDKOŚCI
        // Losujemy opóźnienie od 2 (bardzo szybka) do 15 (wolna)
        // rand() % 14 daje zakres 0-13, plus 2 daje zakres 2-15.
        int random_speed = (rand() % 14) + 2;

        current_star->speed_delay = random_speed;
        current_star->move_timer = random_speed;

        (*star_index_to_spawn)++;
    }
}

void update_stars(WINDOW *gameScreen, Star stars[], Swallow *swallow, Stats *stats)
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        Star *star = &stars[i];

        if (star->is_active)
        {
            // KOLIZJA Z JASKÓŁKĄ
            if (star->x == swallow->x && star->y == swallow->y)
            {
                mvwprintw(gameScreen, star->y, star->x, " ");
                stats->score++;
                star->is_active = 0;
            }
            // RUCH GWIAZDY
            else if (star->move_timer <= 0)
            {
                int prev_y = star->y;
                star->y++;

                // Czyszczenie starej pozycji
                if (prev_y < GAME_SCREEN_HEIGHT - 1)
                {
                    mvwprintw(gameScreen, prev_y, star->x, " ");
                }

                // RESET TIMERA DO INDYWIDUALNEJ PRĘDKOŚCI GWIAZDY
                // Zamiast sztywnego '= 10', używamy wylosowanej wartości
                star->move_timer = star->speed_delay;
            }
            else
            {
                star->move_timer--;
            }

            // SPRAWDZANIE GRANIC I RYSOWANIE
            if (star->y >= GAME_SCREEN_HEIGHT - 1)
            {
                stats->starsFumbled++;
                star->is_active = 0;
            }
            else if (star->is_active)
            {
                mvwprintw(gameScreen, star->y, star->x, "%s", star->sign);
            }
        }
    }
}