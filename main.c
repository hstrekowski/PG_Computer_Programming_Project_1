#include <ncurses.h>
#include "game.h"
#include "swallow.h"
#include "render.h"
#include "config.h"

// Konfiguracja ncurses i kolorów
static void setup_ncurses()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    // Inicjalizacja kolorów
    if (has_colors())
    {
        start_color();
        use_default_colors();
        init_pair(PAIR_WHITE, COLOR_WHITE, -1);
        init_pair(PAIR_ORANGE, COLOR_YELLOW, -1);
        init_pair(PAIR_RED, COLOR_RED, -1);
        init_pair(PAIR_HUNTER_GREEN, COLOR_GREEN, -1);
        init_pair(PAIR_HUNTER_CYAN, COLOR_CYAN, -1);
        init_pair(PAIR_HUNTER_MAGENTA, COLOR_MAGENTA, -1);
        init_pair(PAIR_HUNTER_BLUE, COLOR_BLUE, -1);
    }
}

// Tworzenie okien gry
static void setup_windows(WINDOW **win, WINDOW **game, WINDOW **stat)
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Głowne okno
    *win = newwin(WINDOW_HEIGHT, WINDOW_WIDTH,
                  (max_y - WINDOW_HEIGHT) / 2 - 1,
                  (max_x - WINDOW_WIDTH) / 2 - 1);
    box(*win, 0, 0);

    // Okno gry
    *game = newwin(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH,
                   ((max_y - WINDOW_HEIGHT) / 2) - (STATUS_AREA_HEIGHT - STATUS_AREA_HEIGHT),
                   (max_x - WINDOW_WIDTH) / 2);
    box(*game, 0, 0);
    nodelay(*game, TRUE);

    // Okno ze statusem
    *stat = newwin(STATUS_AREA_HEIGHT, STATUS_AREA_WIDTH,
                   ((max_y - WINDOW_HEIGHT) / 2) + (GAME_SCREEN_HEIGHT),
                   (max_x - WINDOW_WIDTH) / 2);
    box(*stat, 0, 0);
}

// Główny punkt wejścia
int main()
{
    setup_ncurses();

    WINDOW *window, *gameScreen, *statusArea;
    setup_windows(&window, &gameScreen, &statusArea);

    WINDOW *windows[] = {window, gameScreen, statusArea};
    refresh();
    refresh_windows(windows, 3);

    Swallow my_swallow;
    init_swallow(&my_swallow);

    PlayerConfig config;
    show_start_screen(gameScreen, &config);
    nodelay(gameScreen, TRUE);

    run_game_loop(gameScreen, statusArea, &my_swallow, &config);

    // Zakończenie programu, czyszczenie okien, wyjscie z trybu ncurses
    nodelay(gameScreen, FALSE);
    refresh_windows(windows, 3);
    endwin();

    return 0;
}