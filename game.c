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

// NOWE: Funkcja ekranu startowego
void show_start_screen(WINDOW *gameScreen, PlayerConfig *config)
{
    // 1. Włączamy kursor i echo, żeby widzieć wpisywanie
    echo();
    curs_set(1);

    // 2. Obliczamy środek dla boxa
    int box_h = 10;
    int box_w = 50;
    int start_y = (GAME_SCREEN_HEIGHT - box_h) / 2;
    int start_x = (GAME_SCREEN_WIDTH - box_w) / 2;

    // 3. Tworzymy tymczasowe okno wewnątrz gameScreen (derwin)
    WINDOW *inputWin = derwin(gameScreen, box_h, box_w, start_y, start_x);
    box(inputWin, 0, 0); // Ramka

    // Tytuł w ramce
    const char *title = " GAME SETUP ";
    mvwprintw(inputWin, 0, (box_w - strlen(title)) / 2, "%s", title);

    // 4. Pole NICK
    mvwprintw(inputWin, 3, 4, "Enter your Nickname:");
    wrefresh(inputWin);

    // Przesuwamy kursor i pobieramy string
    mvwgetnstr(inputWin, 4, 4, config->name, 29); // Max 29 znaków

    // 5. Pole POZIOM
    mvwprintw(inputWin, 6, 4, "Enter Level (1-10):");
    wrefresh(inputWin);

    // Pobieramy poziom jako string, a potem konwertujemy na int
    char level_str[10];
    mvwgetnstr(inputWin, 7, 4, level_str, 9);
    config->startLevel = atoi(level_str); // atoi zamienia string na int

    // 6. Sprzątanie
    delwin(inputWin);      // Usuwamy okienko inputu
    wclear(gameScreen);    // Czyścimy ekran gry ze śmieci po inputcie
    box(gameScreen, 0, 0); // Przywracamy ramkę gry
    wrefresh(gameScreen);

    // Wyłączamy kursor i echo (tryb gry)
    noecho();
    curs_set(0);
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow, PlayerConfig *config)
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

    wattron(gameScreen, COLOR_PAIR(PAIR_WHITE));
    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wattroff(gameScreen, COLOR_PAIR(PAIR_WHITE));
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

    sprintf(msg_stats, "Player: %s  |  Score: %d  |  Lives: %d", config->name, stats.score, final_lives);

    int center_y = GAME_SCREEN_HEIGHT / 2;
    int center_x = GAME_SCREEN_WIDTH / 2;

    mvwprintw(gameScreen, center_y - 1, center_x - (strlen(msg_game_over) / 2), "%s", msg_game_over);
    mvwprintw(gameScreen, center_y + 1, center_x - (strlen(msg_stats) / 2), "%s", msg_stats);

    wrefresh(gameScreen);
}