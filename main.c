#include <ncurses.h>

#define WINDOW_HEIGHT 36
#define WINDOW_WIDTH 134

#define GAME_SCREEN_HEIGHT 27
#define GAME_SCREEN_WIDTH 132

#define STATUS_AREA_HEIGHT 7
#define STATUS_AREA_WIDTH 132

int main()
{
    // Init
    initscr();
    cbreak();
    noecho();

    // Window Setup
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *window = newwin(WINDOW_HEIGHT,
                            WINDOW_WIDTH,
                            (max_y - WINDOW_HEIGHT) / 2 - 1,
                            (max_x - WINDOW_WIDTH) / 2 - 1);

    box(window, 0, 0);

    WINDOW *gameScreen = newwin(GAME_SCREEN_HEIGHT,
                                GAME_SCREEN_WIDTH,
                                ((max_y - WINDOW_HEIGHT) / 2) - (STATUS_AREA_HEIGHT - STATUS_AREA_HEIGHT),
                                (max_x - WINDOW_WIDTH) / 2);

    box(gameScreen, 0, 0);

    WINDOW *statusArea = newwin(STATUS_AREA_HEIGHT,
                                STATUS_AREA_WIDTH,
                                ((max_y - WINDOW_HEIGHT) / 2) + (GAME_SCREEN_HEIGHT),
                                (max_x - WINDOW_WIDTH) / 2);

    box(statusArea, 0, 0);

    // for (int i = 1; i < STATUS_AREA_HEIGHT - 1; i++)
    // {
    //     for (int j = 1; j < STATUS_AREA_WIDTH - 1; j++)
    //     {
    //         mvwprintw(statusArea, i, j, "*");
    //     }
    // }

    // for (int i = 1; i < GAME_SCREEN_HEIGHT - 1; i++)
    // {
    //     for (int j = 1; j < GAME_SCREEN_WIDTH - 1; j++)
    //     {
    //         mvwprintw(gameScreen, i, j, "/");
    //     }
    // }

    refresh();
    wrefresh(window);
    wrefresh(gameScreen);
    wrefresh(statusArea);

    // End
    getch();
    delwin(window);
    endwin();

    return 0;
}