#include "hunter.h"
#include <stdlib.h>
#include <math.h> // Potrzebne do pierwiastka (sqrt)

void init_hunters(Hunter hunters[])
{
    for (int i = 0; i < MAX_HUNTERS; i++)
    {
        hunters[i].is_active = 0;
    }
}

void try_spawn_hunter(Hunter hunters[], Swallow *swallow, int frame_counter)
{
    // Spawnuj Huntera co np. 100 klatek (ok. 2 sekundy)
    if (frame_counter % 100 != 0)
        return;

    // Znajdź wolny slot
    int slot = -1;
    for (int i = 0; i < MAX_HUNTERS; i++)
    {
        if (!hunters[i].is_active)
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
        return; // Brak miejsca

    Hunter *h = &hunters[slot];
    h->is_active = 1;

    // 1. Losowanie wymiarów (1x2, 2x1, 1x3, 3x1, 2x2)
    int shape_type = rand() % 5;
    switch (shape_type)
    {
    case 0:
        h->width = 1;
        h->height = 2;
        break;
    case 1:
        h->width = 2;
        h->height = 1;
        break;
    case 2:
        h->width = 1;
        h->height = 3;
        break;
    case 3:
        h->width = 3;
        h->height = 1;
        break;
    case 4:
        h->width = 2;
        h->height = 2;
        break;
    }

    // 2. Losowanie odbić (1 do 3)
    h->bounces_left = (rand() % 3) + 1;

    // 3. Losowanie rogu startowego
    int corner = rand() % 4;
    switch (corner)
    {
    case 0:
        h->x = 2;
        h->y = 2;
        break; // Lewy góra
    case 1:
        h->x = GAME_SCREEN_WIDTH - 3;
        h->y = 2;
        break; // Prawy góra
    case 2:
        h->x = 2;
        h->y = GAME_SCREEN_HEIGHT - 3;
        break; // Lewy dół
    case 3:
        h->x = GAME_SCREEN_WIDTH - 3;
        h->y = GAME_SCREEN_HEIGHT - 3;
        break; // Prawy dół
    }

    // 4. Obliczanie wektora w stronę Swallow (celowanie)
    float target_x = (float)swallow->x;
    float target_y = (float)swallow->y;

    float diff_x = target_x - h->x;
    float diff_y = target_y - h->y;

    // Twierdzenie Pitagorasa dla długości wektora
    float length = sqrt(diff_x * diff_x + diff_y * diff_y);

    // Normalizacja (żeby długość skoku była stała) i pomnożenie przez prędkość
    // Prędkość huntera = 0.5 kratki na klatkę (płynniej, ale wolniej niż max jaskółki)
    float speed = 0.5f;

    if (length != 0)
    {
        h->dx = (diff_x / length) * speed;
        h->dy = (diff_y / length) * speed;
    }
    else
    {
        h->dx = speed;
        h->dy = speed;
    }
}

void update_hunters(WINDOW *gameScreen, Hunter hunters[], Swallow *swallow)
{
    for (int i = 0; i < MAX_HUNTERS; i++)
    {
        Hunter *h = &hunters[i];
        if (!h->is_active)
            continue;

        // --- CZYSZCZENIE STAREJ POZYCJI ---
        for (int ry = 0; ry < h->height; ry++)
        {
            for (int rx = 0; rx < h->width; rx++)
            {
                int draw_y = (int)h->y + ry;
                int draw_x = (int)h->x + rx;
                if (draw_y > 0 && draw_y < GAME_SCREEN_HEIGHT - 1 && draw_x > 0 && draw_x < GAME_SCREEN_WIDTH - 1)
                    mvwprintw(gameScreen, draw_y, draw_x, " ");
            }
        }

        // --- RUCH ---
        h->x += h->dx;
        h->y += h->dy;

        // --- ODBICIA ---
        int bounced = 0;

        // Odbicie poziome
        if (h->x <= 1)
        {
            h->x = 1.1f;
            h->dx = -h->dx;
            bounced = 1;
        }
        else if (h->x + h->width >= GAME_SCREEN_WIDTH - 1)
        {
            h->x = GAME_SCREEN_WIDTH - 1 - h->width - 0.1f;
            h->dx = -h->dx;
            bounced = 1;
        }

        // Odbicie pionowe
        if (h->y <= 1)
        {
            h->y = 1.1f;
            h->dy = -h->dy;
            bounced = 1;
        }
        else if (h->y + h->height >= GAME_SCREEN_HEIGHT - 1)
        {
            h->y = GAME_SCREEN_HEIGHT - 1 - h->height - 0.1f;
            h->dy = -h->dy;
            bounced = 1;
        }

        // LOGIKA ZNIKANIA
        if (bounced)
        {
            h->bounces_left--;

            // ZMIANA TUTAJ:
            // Jeśli po odjęciu zostało 0 (czyli odbił się mając 1), to znika OD RAZU.
            // Nigdy nie wyświetli się "0".
            if (h->bounces_left == 0)
            {
                h->is_active = 0;
                continue; // Przerwij pętlę dla tego huntera, nie rysuj go więcej
            }
        }

        // --- KOLIZJA ZE SWALLOW ---
        int hit = 0;
        int sx = swallow->x;
        int sy = swallow->y;

        if (sx >= (int)h->x && sx < (int)h->x + h->width &&
            sy >= (int)h->y && sy < (int)h->y + h->height)
        {

            if (swallow->lifeForce > 0)
            {
                swallow->lifeForce--;
            }
            h->is_active = 0;
            hit = 1;
        }

        // --- RYSOWANIE ---
        if (!hit && h->is_active)
        {
            for (int ry = 0; ry < h->height; ry++)
            {
                for (int rx = 0; rx < h->width; rx++)
                {
                    int draw_y = (int)h->y + ry;
                    int draw_x = (int)h->x + rx;
                    mvwprintw(gameScreen, draw_y, draw_x, "%d", h->bounces_left);
                }
            }
        }
    }
}