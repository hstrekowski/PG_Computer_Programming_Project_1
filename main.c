#include <ncurses.h>
#include "game.h"
#include "swallow.h"

int main()
{
    // Init
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    // Colors Setup
    if (has_colors())
    {
        start_color();
        use_default_colors();

        // Swallow -  ID, text, background
        init_pair(PAIR_WHITE, COLOR_WHITE, -1);
        init_pair(PAIR_ORANGE, COLOR_YELLOW, -1);
        init_pair(PAIR_RED, COLOR_RED, -1);

        // Hunter - ID, text, background
        init_pair(PAIR_HUNTER_GREEN, COLOR_GREEN, -1);
        init_pair(PAIR_HUNTER_CYAN, COLOR_CYAN, -1);
        init_pair(PAIR_HUNTER_MAGENTA, COLOR_MAGENTA, -1);
        init_pair(PAIR_HUNTER_BLUE, COLOR_BLUE, -1);
    }

    // Window Setup
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *window = newwin(WINDOW_HEIGHT, WINDOW_WIDTH,
                            (max_y - WINDOW_HEIGHT) / 2 - 1,
                            (max_x - WINDOW_WIDTH) / 2 - 1);
    box(window, 0, 0);

    WINDOW *gameScreen = newwin(GAME_SCREEN_HEIGHT, GAME_SCREEN_WIDTH,
                                ((max_y - WINDOW_HEIGHT) / 2) - (STATUS_AREA_HEIGHT - STATUS_AREA_HEIGHT),
                                (max_x - WINDOW_WIDTH) / 2);
    box(gameScreen, 0, 0);
    nodelay(gameScreen, TRUE);

    WINDOW *statusArea = newwin(STATUS_AREA_HEIGHT, STATUS_AREA_WIDTH,
                                ((max_y - WINDOW_HEIGHT) / 2) + (GAME_SCREEN_HEIGHT),
                                (max_x - WINDOW_WIDTH) / 2);
    box(statusArea, 0, 0);

    WINDOW *windows[] = {window, gameScreen, statusArea};

    refresh();
    refresh_windows(windows, 3);

    // Game Init
    Swallow my_swallow;
    init_swallow(&my_swallow);
    run_game_loop(gameScreen, statusArea, &my_swallow);

    // End
    nodelay(gameScreen, FALSE);
    getch();
    refresh_windows(windows, 3);
    endwin();

    return 0;
}