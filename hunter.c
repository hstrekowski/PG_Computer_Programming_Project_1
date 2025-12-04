#include "hunter.h"
#include <stdlib.h>
#include <math.h>

void init_hunters(Hunter hunters[])
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        hunters[i].is_active = 0;
    }
}

void try_spawn_hunter(Hunter hunters[], Swallow *swallow, int frame_counter, int spawn_freq, int max_hunters, int allowed_types[5])
{
    if (frame_counter % spawn_freq != 0)
        return;

    // Sprawdź czy jakikolwiek typ jest dozwolony, żeby uniknąć nieskończonej pętli
    int any_allowed = 0;
    for (int i = 0; i < 5; i++)
        if (allowed_types[i])
            any_allowed = 1;
    if (!any_allowed)
        return; // Jeśli konfiguracja zabrania wszystkich, nie spawnuj

    int active_count = 0;
    int slot = -1;
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        if (hunters[i].is_active)
            active_count++;
        else if (slot == -1)
            slot = i;
    }

    if (active_count >= max_hunters || slot == -1)
        return;

    Hunter *h = &hunters[slot];
    h->is_active = 1;

    // Losujemy typ dopóki nie trafimy na dozwolony
    int shape_type;
    do
    {
        shape_type = rand() % 5;
    } while (allowed_types[shape_type] == 0);

    switch (shape_type)
    {
    case 0:
        h->width = 1;
        h->height = 2;
        h->color_pair = PAIR_HUNTER_GREEN;
        break;
    case 1:
        h->width = 2;
        h->height = 1;
        h->color_pair = PAIR_HUNTER_CYAN;
        break;
    case 2:
        h->width = 1;
        h->height = 3;
        h->color_pair = PAIR_HUNTER_MAGENTA;
        break;
    case 3:
        h->width = 3;
        h->height = 1;
        h->color_pair = PAIR_HUNTER_BLUE;
        break;
    case 4:
        h->width = 2;
        h->height = 2;
        h->color_pair = PAIR_RED;
        break;
    }

    h->bounces_left = (rand() % 3) + 1;
    int corner = rand() % 4;
    switch (corner)
    {
    case 0:
        h->x = 2;
        h->y = 2;
        break;
    case 1:
        h->x = GAME_SCREEN_WIDTH - 3;
        h->y = 2;
        break;
    case 2:
        h->x = 2;
        h->y = GAME_SCREEN_HEIGHT - 3;
        break;
    case 3:
        h->x = GAME_SCREEN_WIDTH - 3;
        h->y = GAME_SCREEN_HEIGHT - 3;
        break;
    }

    float target_x = (float)swallow->x;
    float target_y = (float)swallow->y;
    float diff_x = target_x - h->x;
    float diff_y = target_y - h->y;
    float length = sqrt(diff_x * diff_x + diff_y * diff_y);
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

void update_hunters(WINDOW *gameScreen, Hunter hunters[], Swallow *swallow, int damage)
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        Hunter *h = &hunters[i];
        if (!h->is_active)
            continue;

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
        h->x += h->dx;
        h->y += h->dy;

        int bounced = 0;
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

        if (bounced)
        {
            h->bounces_left--;
            if (h->bounces_left == 0)
            {
                h->is_active = 0;
                continue;
            }
        }

        int hit = 0;
        int sx = swallow->x;
        int sy = swallow->y;
        if (sx >= (int)h->x && sx < (int)h->x + h->width && sy >= (int)h->y && sy < (int)h->y + h->height)
        {

            // ZMIANA: Odejmujemy tyle, ile zdefiniowano w damage
            if (swallow->lifeForce > 0)
            {
                swallow->lifeForce -= damage;
            }
            h->is_active = 0;
            hit = 1;
        }

        if (!hit && h->is_active)
        {
            wattron(gameScreen, COLOR_PAIR(h->color_pair));
            for (int ry = 0; ry < h->height; ry++)
            {
                for (int rx = 0; rx < h->width; rx++)
                {
                    mvwprintw(gameScreen, (int)h->y + ry, (int)h->x + rx, "%d", h->bounces_left);
                }
            }
            wattroff(gameScreen, COLOR_PAIR(h->color_pair));
        }
    }
}