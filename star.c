#include "star.h"
#include <stdlib.h> // dla rand()

void init_stars(Star stars[])
{
    for (int i = 0; i < MAX_STARS; i++)
    {
        stars[i].is_active = 0;
        stars[i].sign = "*";
        stars[i].x = 0;
        stars[i].y = 0;
        stars[i].move_timer = 0;
    }
}

void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter)
{
    const int STAR_SPAWN_FREQUENCY_FRAMES = 15;

    if (frame_counter % STAR_SPAWN_FREQUENCY_FRAMES == 0 && *star_index_to_spawn < MAX_STARS)
    {
        Star *current_star = &stars[*star_index_to_spawn];
        current_star->is_active = 1;
        current_star->x = (rand() % (GAME_SCREEN_WIDTH - 2)) + 1;
        current_star->y = 1;
        current_star->move_timer = 10;

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
                star->move_timer = 10;
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