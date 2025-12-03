#include "game.h"
#include "swallow.h"
#include "star.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void refresh_windows(WINDOW *windows[], int n)
{
    for (int i = 0; i < n; i++)
    {
        wrefresh(windows[i]);
    }
}

void update_status(WINDOW *statusArea, int score, int seconds, int speed, int starsFumbled)
{
    mvwprintw(statusArea, 1, 1, "Score: %d     ", score);
    mvwprintw(statusArea, 3, 1, "Seconds left: %d   ", seconds);
    mvwprintw(statusArea, 5, 1, "Your speed %d     ", speed);
    mvwprintw(statusArea, 7, 1, "Stars fumbled: %d   ", starsFumbled);
    wrefresh(statusArea);
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow)
{
    Stats stats = {0, 0};

    // Obliczenie całkowitej liczby klatek: 60 * 45 = 2700
    int total_frames = DURATION_SECONDS * FRAME_RATE;
    int frame_counter = 0;
    const int SLEEP_TIME_US = 1000000 / FRAME_RATE;

    int move_counter = 10;
    int star_index_to_spawn = 0;

    srand(time(NULL));

    // Inicjalizacja gwiazd
    Star stars[MAX_STARS];
    init_stars(stars);

    // Rysowanie jaskółki na start
    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wrefresh(gameScreen);

    // Pierwsze wyświetlenie statusu (żeby od razu pokazało 60s, a nie czekało na klatkę)
    update_status(statusArea, stats.score, total_frames / FRAME_RATE, swallow->speed, stats.starsFumbled);

    while (total_frames > 0)
    {
        // 1. INPUT
        int should_quit = handle_input(gameScreen, swallow);
        if (should_quit)
            break;

        // 2. RUCH JASKÓŁKI
        update_swallow_position(gameScreen, swallow, &move_counter);

        // 3. LOGIKA GWIAZD
        try_spawn_star(stars, &star_index_to_spawn, frame_counter);

        // Logika gwiazd + aktualizacja statusu przy trafieniu
        // Musimy to lekko zmodyfikować, bo update_stars w poprzedniej wersji nie miał dostępu do total_frames
        // Najprościej: przenieść logikę update_status TUTAJ, albo zaktualizować status zawsze na końcu pętli.

        // Wykonaj logikę gwiazd (ruch, kolizje)
        update_stars(gameScreen, stars, swallow, &stats);

        // 4. UI UPDATE
        // TUTAJ BYŁ BŁĄD: Zmieniono '60' na 'FRAME_RATE'
        update_status(statusArea, stats.score, total_frames / FRAME_RATE, swallow->speed, stats.starsFumbled);

        wrefresh(gameScreen);

        // 5. TIMING
        usleep(SLEEP_TIME_US);
        total_frames--;
        frame_counter++;
        move_counter--;
    }
}