#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_HEIGHT 36
#define WINDOW_WIDTH 134

#define GAME_SCREEN_HEIGHT 27
#define GAME_SCREEN_WIDTH 132

#define STATUS_AREA_HEIGHT 7
#define STATUS_AREA_WIDTH 132

#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96

typedef struct Swallow
{
    int x;
    int y;
    int speed;
    int direction;
    int lifeForce;
    char *sign;
} Swallow;

typedef struct Star
{
    int x;
    int y;
    char *sign;
    int active; // 1 = aktywna, 0 = nieaktywna
} Star;

void refresh_windows(WINDOW *windows[], int n)
{
    for (int i = 0; i < n; i++)
    {
        wrefresh(windows[i]);
    }
}

void run_game_loop(WINDOW *gameScreen, WINDOW *statusArea, Swallow *swallow)
{
    const int DURATION_SECONDS = 60;
    const int FRAME_RATE = 45;
    const int SLEEP_TIME_US = 1000000 / FRAME_RATE;

    int total_frames = DURATION_SECONDS * FRAME_RATE;
    int move_counter = 0;

    move_counter = 10;

    // Rysowanie jaskółki na początku (jeśli nie jest w main)
    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wrefresh(gameScreen);

    // Inicjalizacja generatora liczb losowych
    srand(time(NULL));

    while (total_frames > 0)
    {
        // 1. ASYNCHRONICZNE ODBIERANIE INPUTU
        int ch = wgetch(gameScreen);

        if (ch != ERR)
        {
            switch (ch)
            {
            case 'w':
            case 'W':
                swallow->direction = UP;
                break;
            case 's':
            case 'S':
                swallow->direction = DOWN;
                break;
            case 'a':
            case 'A':
                swallow->direction = LEFT;
                break;
            case 'd':
            case 'D':
                swallow->direction = RIGHT;
                break;
            case 'q':
            case 'Q':
                total_frames = 0;
                break;
            case 'o':
            case 'O':
                swallow->speed--;
                break;
            case 'p':
            case 'P':
                swallow->speed++;
                break;
            default:
                break;
            }
        }

        // 2. LOGIKA RUCHU JASKÓŁKI (co sekundę, move_counter = 10)
        if (move_counter == 0)
        {

            int prev_y = swallow->y;
            int prev_x = swallow->x;

            switch (swallow->direction)
            {
            case UP:
                swallow->y -= swallow->speed;
                break;
            case DOWN:
                swallow->y += swallow->speed;
                break;
            case LEFT:
                swallow->x -= swallow->speed;
                break;
            case RIGHT:
                swallow->x += swallow->speed;
                break;
            default:
                break;
            }

            mvwprintw(gameScreen, prev_y, prev_x, "%s", " ");

            // Sprawdzanie granic
            if (swallow->y < 1)
                swallow->y = 1;
            if (swallow->y > GAME_SCREEN_HEIGHT - 2)
                swallow->y = GAME_SCREEN_HEIGHT - 2;
            if (swallow->x < 1)
                swallow->x = 1;
            if (swallow->x > GAME_SCREEN_WIDTH - 2)
                swallow->x = GAME_SCREEN_WIDTH - 2;

            mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);

            move_counter = 10;
        }

        // 4. ODŚWIEŻANIE I OCZEKIWANIE
        wrefresh(gameScreen);
        usleep(SLEEP_TIME_US);

        // Aktualizacja liczników
        total_frames--;
        move_counter--;
    }
}

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
    nodelay(gameScreen, TRUE);

    WINDOW *statusArea = newwin(STATUS_AREA_HEIGHT,
                                STATUS_AREA_WIDTH,
                                ((max_y - WINDOW_HEIGHT) / 2) + (GAME_SCREEN_HEIGHT),
                                (max_x - WINDOW_WIDTH) / 2);

    box(statusArea, 0, 0);

    WINDOW *windows[] = {window, gameScreen, statusArea};

    refresh();
    refresh_windows(windows, 3);

    Swallow my_swallow = {
        .x = GAME_SCREEN_WIDTH / 2,
        .y = GAME_SCREEN_HEIGHT - 3,
        .direction = UP,
        .sign = "^",
        .speed = 1,
        .lifeForce = 100};

    run_game_loop(gameScreen, statusArea, &my_swallow);

    // End
    getch();
    refresh_windows(windows, 3);
    endwin();

    return 0;
}