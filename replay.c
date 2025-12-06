#include "replay.h"
#include "render.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void init_replay(ReplaySystem *r)
{
    // Alokujemy pamięć na stercie (heap), bo na stosie (stack) mogłoby braknąć miejsca
    r->frames = (ReplayFrame *)malloc(sizeof(ReplayFrame) * MAX_REPLAY_FRAMES);
    r->frame_count = 0;
    r->max_frames = MAX_REPLAY_FRAMES;
}

void free_replay(ReplaySystem *r)
{
    if (r->frames)
    {
        free(r->frames);
        r->frames = NULL;
    }
}

void record_frame(ReplaySystem *r, Swallow *s, Star stars[], Hunter hunters[], SafeZone *sz, Stats *st, int lives, int f_left)
{
    if (r->frame_count >= r->max_frames)
        return;

    ReplayFrame *f = &r->frames[r->frame_count];

    // Kopiujemy struktury (C pozwala na przypisanie struct = struct)
    f->swallow = *s;
    f->safeZone = *sz;
    f->stats = *st;
    f->lifeForce = lives;
    f->frames_left = f_left;

    // Kopiujemy tablice (używamy memcpy dla szybkości)
    memcpy(f->stars, stars, sizeof(Star) * MAX_STARS_LIMIT);
    memcpy(f->hunters, hunters, sizeof(Hunter) * MAX_HUNTERS_LIMIT);

    r->frame_count++;
}

void play_replay(ReplaySystem *r, WINDOW *gameWin, WINDOW *statWin, PlayerConfig *p, LevelConfig *l)
{
    const int SLEEP = 1000000 / FRAME_RATE;

    // Ustawiamy tryb non-blocking dla inputu
    nodelay(gameWin, TRUE);

    for (int i = 0; i < r->frame_count; i++)
    {
        // Obsługa wyjścia 'Q'
        int ch = wgetch(gameWin);
        if (ch == 'q' || ch == 'Q')
            break;

        // Pobieramy klatkę
        ReplayFrame *f = &r->frames[i];

        // --- RYSOWANIE ---
        // Używamy istniejących funkcji z render.c i modułów,
        // przekazując im dane z klatki powtórki.

        // 1. Safe Zone
        draw_safe_zone(gameWin, &f->safeZone);

        // 2. Swallow (musimy wyczyścić tło, bo renderery tego nie robią globalnie)
        wclear(gameWin);
        box(gameWin, 0, 0); // Ramka gry

        // Rysowanie Safe Zone (ponownie, bo clear wyczyścił)
        draw_safe_zone(gameWin, &f->safeZone);

        // Rysowanie Jaskółki
        // Musimy odtworzyć kolorowanie jaskółki (logika z swallow.c przeniesiona wizualnie)
        int color = PAIR_WHITE;
        if (f->safeZone.is_active || f->safeZone.duration_timer > 0)
            color = PAIR_ORANGE; // W strefie
        else if (f->lifeForce == 2)
            color = PAIR_ORANGE;
        else if (f->lifeForce <= 1)
            color = PAIR_RED;

        wattron(gameWin, COLOR_PAIR(color));
        mvwprintw(gameWin, f->swallow.y, f->swallow.x, "%s", f->swallow.sign);
        wattroff(gameWin, COLOR_PAIR(color));

        // 3. Gwiazdy (używamy logiki z star.c, ale musimy zreimplementować proste rysowanie
        // lub wywołać funkcję z star.c. Ponieważ star.c w draw_animated zmienia stan (tickery),
        // lepiej narysować je tutaj "na sztywno" używając danych z nagrania).
        for (int j = 0; j < MAX_STARS_LIMIT; j++)
        {
            if (f->stars[j].is_active)
            {
                // Logika koloru gwiazdy (skopiowana dla wizualizacji)
                int s_color = PAIR_WHITE;
                if (f->stars[j].y > (GAME_SCREEN_HEIGHT / 3) * 2)
                    s_color = PAIR_RED;
                else if (f->stars[j].y > GAME_SCREEN_HEIGHT / 3)
                    s_color = PAIR_ORANGE;

                wattron(gameWin, COLOR_PAIR(s_color));
                mvwprintw(gameWin, f->stars[j].y, f->stars[j].x, "%s", f->stars[j].sign);
                wattroff(gameWin, COLOR_PAIR(s_color));
            }
        }

        // 4. Hunterzy
        for (int j = 0; j < MAX_HUNTERS_LIMIT; j++)
        {
            if (f->hunters[j].is_active)
            {
                Hunter *h = &f->hunters[j];
                wattron(gameWin, COLOR_PAIR(h->color_pair));
                for (int ry = 0; ry < h->height; ry++)
                {
                    for (int rx = 0; rx < h->width; rx++)
                    {
                        mvwprintw(gameWin, (int)h->y + ry, (int)h->x + rx, "%d", h->bounces_left);
                    }
                }
                wattroff(gameWin, COLOR_PAIR(h->color_pair));
            }
        }

        // 5. Status
        draw_status(statWin, p, l, &f->stats, f->lifeForce, f->frames_left / FRAME_RATE, f->swallow.speed, &f->safeZone);

        // Nadpisujemy fragment statusu informacją o REPLAYU
        wattron(statWin, COLOR_PAIR(PAIR_HUNTER_MAGENTA));
        mvwprintw(statWin, 5, 2, "[ REPLAY MODE ]");
        mvwprintw(statWin, 6, 90, "Q - STOP REPLAY");
        wattroff(statWin, COLOR_PAIR(PAIR_HUNTER_MAGENTA));
        wrefresh(statWin);

        wrefresh(gameWin);
        usleep(SLEEP);
    }
}