#include "hunter.h"
#include <stdlib.h>
#include <math.h>

void init_hunters(Hunter hunters[])
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        hunters[i].is_active = 0;
        hunters[i].has_dashed = 0;
    }
}

void try_spawn_hunter(Hunter hunters[], Swallow *swallow, int frame_counter, int total_frames, int spawn_freq, int max_hunters, int allowed_types[5])
{
    if (frame_counter % spawn_freq != 0)
        return;

    int dynamic_max_hunters = max_hunters;
    int remaining_frames = total_frames - frame_counter;

    if (remaining_frames <= total_frames / 4)
    {
        // Ostatnia ćwiartka czasu: limit + 2
        dynamic_max_hunters += 2;
    }
    else if (remaining_frames <= total_frames / 2)
    {
        // Druga połowa czasu: limit + 1
        dynamic_max_hunters += 1;
    }

    if (dynamic_max_hunters > MAX_HUNTERS_LIMIT)
    {
        dynamic_max_hunters = MAX_HUNTERS_LIMIT;
    }

    int any_allowed = 0;
    for (int i = 0; i < 5; i++)
        if (allowed_types[i])
            any_allowed = 1;
    if (!any_allowed)
        return;

    int active_count = 0;
    int slot = -1;
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        if (hunters[i].is_active)
            active_count++;
        else if (slot == -1)
            slot = i;
    }

    if (active_count >= dynamic_max_hunters || slot == -1)
        return;

    Hunter *h = &hunters[slot];
    h->is_active = 1;
    h->has_dashed = 0;

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

    // --- NOWA LOGIKA ODBIĆ ---
    int min_bounces = 1; // Domyślnie 1-3

    if (remaining_frames <= total_frames / 4)
    {
        // Mniej niż 1/4 czasu (końcówka): 3-5 odbić
        min_bounces = 3;
    }
    else if (remaining_frames <= total_frames / 2)
    {
        // Mniej niż 1/2 czasu: 2-4 odbić
        min_bounces = 2;
    }

    // Losujemy z zakresu o szerokości 3 (np. 1-3, 2-4, 3-5)
    // rand() % 3 daje 0,1,2. Dodajemy min_bounces.
    h->bounces_left = (rand() % 3) + min_bounces;

    // --- Reszta (pozycja i wektor) bez zmian ---
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

void update_hunters(WINDOW *gameScreen, Hunter hunters[], Swallow *swallow, int damage, SafeZone *safeZone)
{
    for (int i = 0; i < MAX_HUNTERS_LIMIT; i++)
    {
        Hunter *h = &hunters[i];
        if (!h->is_active)
            continue;

        // Czyszczenie
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

        // ... (Logika odbić i dasha BEZ ZMIAN - skopiuj z poprzednich wersji) ...
        int bounced = 0;
        int hit_horizontal = 0;
        int hit_vertical = 0;
        if (h->x <= 1)
        {
            h->x = 1.1f;
            hit_horizontal = 1;
            bounced = 1;
        }
        else if (h->x + h->width >= GAME_SCREEN_WIDTH - 1)
        {
            h->x = GAME_SCREEN_WIDTH - 1 - h->width - 0.1f;
            hit_horizontal = 1;
            bounced = 1;
        }
        if (h->y <= 1)
        {
            h->y = 1.1f;
            hit_vertical = 1;
            bounced = 1;
        }
        else if (h->y + h->height >= GAME_SCREEN_HEIGHT - 1)
        {
            h->y = GAME_SCREEN_HEIGHT - 1 - h->height - 0.1f;
            hit_vertical = 1;
            bounced = 1;
        }
        if (bounced)
        {
            if (!h->has_dashed)
            {
                float potential_dx = h->dx;
                float potential_dy = h->dy;
                if (hit_horizontal)
                    potential_dx = -potential_dx;
                if (hit_vertical)
                    potential_dy = -potential_dy;
                float to_swallow_x = (float)swallow->x - h->x;
                float to_swallow_y = (float)swallow->y - h->y;
                float dist = sqrt(to_swallow_x * to_swallow_x + to_swallow_y * to_swallow_y);
                int should_dash = 0;
                if (dist > 0)
                {
                    float potential_len = sqrt(potential_dx * potential_dx + potential_dy * potential_dy);
                    float norm_pot_x = potential_dx / potential_len;
                    float norm_pot_y = potential_dy / potential_len;
                    float norm_swallow_x = to_swallow_x / dist;
                    float norm_swallow_y = to_swallow_y / dist;
                    float dot_product = (norm_pot_x * norm_swallow_x) + (norm_pot_y * norm_swallow_y);
                    if (dot_product < 0.99)
                        should_dash = 1;
                }
                if (should_dash)
                {
                    float dash_speed = 1.5f;
                    h->dx = (to_swallow_x / dist) * dash_speed;
                    h->dy = (to_swallow_y / dist) * dash_speed;
                    h->has_dashed = 1;
                }
                else
                {
                    h->dx = potential_dx;
                    h->dy = potential_dy;
                }
            }
            else
            {
                if (hit_horizontal)
                    h->dx = -h->dx;
                if (hit_vertical)
                    h->dy = -h->dy;
            }
            h->bounces_left--;
            if (h->bounces_left == 0)
            {
                h->is_active = 0;
                continue;
            }
        }
        // ---------------------------------------------------------------

        // --- PRZYWRÓCONA LOGIKA: KOLIZJA Z SAFE ZONE ---
        if (safeZone->is_active)
        {
            // Promień 1 oznacza kwadrat 3x3 (środek +/- 1)
            int zone_radius = 1;

            int zone_left = safeZone->x - zone_radius;
            int zone_right = safeZone->x + zone_radius;
            int zone_top = safeZone->y - zone_radius;
            int zone_bottom = safeZone->y + zone_radius;

            int hunter_left = (int)h->x;
            int hunter_right = (int)h->x + h->width;
            int hunter_top = (int)h->y;
            int hunter_bottom = (int)h->y + h->height;

            if (hunter_left <= zone_right && hunter_right >= zone_left &&
                hunter_top <= zone_bottom && hunter_bottom >= zone_top)
            {

                h->is_active = 0; // Hunter znika po uderzeniu w strefę!
                continue;
            }
        }
        // -----------------------------------------------

        int hit = 0;
        int sx = swallow->x;
        int sy = swallow->y;
        if (sx >= (int)h->x && sx < (int)h->x + h->width && sy >= (int)h->y && sy < (int)h->y + h->height)
        {
            if (!safeZone->is_active)
            {
                if (swallow->lifeForce > 0)
                    swallow->lifeForce -= damage;
                h->is_active = 0;
                hit = 1;
            }
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