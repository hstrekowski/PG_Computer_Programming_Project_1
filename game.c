#include "game.h"
#include "swallow.h"
#include "star.h"
#include "hunter.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void refresh_windows(WINDOW *windows[], int n)
{
    for (int i = 0; i < n; i++)
    {
        wrefresh(windows[i]);
    }
}

void update_status(WINDOW *statusArea, int score, int lifeForce, int seconds, int speed, int starsFumbled)
{
    mvwprintw(statusArea, 1, 1, "Score: %d     ", score);
    mvwprintw(statusArea, 1, 30, "Lives: %d ", lifeForce);
    mvwprintw(statusArea, 3, 1, "Seconds left: %d   ", seconds);
    mvwprintw(statusArea, 5, 1, "Your speed %d     ", speed);
    mvwprintw(statusArea, 7, 1, "Stars fumbled: %d   ", starsFumbled);
    wrefresh(statusArea);
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow)
{
    Stats stats = {0, 0};

    int total_frames = DURATION_SECONDS * FRAME_RATE;
    int frame_counter = 0;
    const int SLEEP_TIME_US = 1000000 / FRAME_RATE;

    int move_counter = BASE_MOVE_RATE;
    int star_index_to_spawn = 0;

    srand(time(NULL));

    // INICJALIZACJA
    Star stars[MAX_STARS];
    init_stars(stars);

    Hunter hunters[MAX_HUNTERS];
    init_hunters(hunters);

    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wrefresh(gameScreen);

    update_status(statusArea, stats.score, swallow->lifeForce, total_frames / FRAME_RATE, swallow->speed, stats.starsFumbled);

    // --- GŁÓWNA PĘTLA GRY ---
    while (total_frames > 0)
    {
        int should_quit = handle_input(gameScreen, swallow);
        if (should_quit)
            break;

        update_swallow_position(gameScreen, swallow, &move_counter);

        try_spawn_star(stars, &star_index_to_spawn, frame_counter);
        update_stars(gameScreen, stars, swallow, &stats);

        try_spawn_hunter(hunters, swallow, frame_counter);
        update_hunters(gameScreen, hunters, swallow);

        if (swallow->lifeForce <= 0)
        {
            break;
        }

        update_status(statusArea, stats.score, swallow->lifeForce, total_frames / FRAME_RATE, swallow->speed, stats.starsFumbled);

        wrefresh(gameScreen);
        usleep(SLEEP_TIME_US);
        total_frames--;
        frame_counter++;
        move_counter--;
    }

    // --- GAME OVER SCREEN ---

    // 1. Wyczyść okno gry
    wclear(gameScreen);
    box(gameScreen, 0, 0); // Opcjonalnie: Rysujemy ramkę ponownie, żeby wyglądało estetycznie

    const char *msg_game_over = "GAME OVER";
    char msg_stats[100];
    int final_lives = swallow->lifeForce < 0 ? 0 : swallow->lifeForce;
    sprintf(msg_stats, "Score: %d  |  Lives: %d  |  Stars Fumbled: %d", stats.score, final_lives, stats.starsFumbled);

    int center_y = GAME_SCREEN_HEIGHT / 2;
    int center_x = GAME_SCREEN_WIDTH / 2;

    mvwprintw(gameScreen, center_y - 1, center_x - (strlen(msg_game_over) / 2), "%s", msg_game_over);
    mvwprintw(gameScreen, center_y + 1, center_x - (strlen(msg_stats) / 2), "%s", msg_stats);

        wrefresh(gameScreen);
}