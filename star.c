#include "star.h"
#include <stdlib.h> // dla rand()

void init_stars(Star stars[])
{
    for (int i = 0; i < MAX_STARS_LIMIT; i++)
    {
        stars[i].is_active = 0;
        stars[i].sign = "*";
        stars[i].x = 0;
        stars[i].y = 0;
        stars[i].move_timer = 0;
        stars[i].speed_delay = 0;
    }
}

void try_spawn_star(Star stars[], int *star_index_to_spawn, int frame_counter, int spawn_freq, int max_stars)
{

    // Sprawdzamy częstotliwość (z pliku poziomu)
    if (frame_counter % spawn_freq == 0)
    {

        // Sprawdzamy limity:
        // 1. Czy nie przekroczyliśmy max liczby gwiazd dla tego poziomu
        // 2. Czy nie przekroczyliśmy rozmiaru tablicy (bezpieczeństwo techniczne)
        if (*star_index_to_spawn < max_stars && *star_index_to_spawn < MAX_STARS_LIMIT)
        {

            Star *current_star = &stars[*star_index_to_spawn];
            current_star->is_active = 1;

            // Losowanie pozycji X (od 1 do WIDTH-2)
            current_star->x = (rand() % (GAME_SCREEN_WIDTH - 2)) + 1;
            current_star->y = 1;

            // Losowanie prędkości (opóźnienia)
            // Zakres np. 2 (szybko) do 15 (wolno)
            int random_speed = (rand() % 14) + 2;
            current_star->speed_delay = random_speed;
            current_star->move_timer = random_speed;

            (*star_index_to_spawn)++;
        }
    }
}

void update_stars(WINDOW *gameScreen, Star stars[], Swallow *swallow, Stats *stats)
{
    // Iterujemy po wszystkich możliwych gwiazdach w tablicy
    for (int i = 0; i < MAX_STARS_LIMIT; i++)
    {
        Star *star = &stars[i];

        if (star->is_active)
        {
            // 1. KOLIZJA Z JASKÓŁKĄ
            if (star->x == swallow->x && star->y == swallow->y)
            {
                mvwprintw(gameScreen, star->y, star->x, " ");
                stats->score++;
                star->is_active = 0;
            }
            // 2. RUCH GWIAZDY
            else if (star->move_timer <= 0)
            {
                int prev_y = star->y;
                star->y++;

                // Czyszczenie starej pozycji
                if (prev_y < GAME_SCREEN_HEIGHT - 1)
                {
                    mvwprintw(gameScreen, prev_y, star->x, " ");
                }

                // Reset timera do indywidualnej prędkości
                star->move_timer = star->speed_delay;
            }
            else
            {
                star->move_timer--;
            }

            // 3. SPRAWDZANIE GRANIC I RYSOWANIE
            if (star->y >= GAME_SCREEN_HEIGHT - 1)
            {
                // Gwiazda spadła na sam dół
                stats->starsFumbled++;
                star->is_active = 0;
            }
            else if (star->is_active)
            {
                // Rysowanie w nowej pozycji
                mvwprintw(gameScreen, star->y, star->x, "%s", star->sign);
            }
        }
    }
}