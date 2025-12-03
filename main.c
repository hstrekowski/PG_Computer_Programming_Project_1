#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Screen constansts
#define WINDOW_HEIGHT 40
#define WINDOW_WIDTH 134

#define GAME_SCREEN_HEIGHT 29
#define GAME_SCREEN_WIDTH 132

#define STATUS_AREA_HEIGHT 9
#define STATUS_AREA_WIDTH 132

// Direction constants
#define UP 99
#define DOWN 98
#define LEFT 97
#define RIGHT 96

// Swallow sign constants
#define UP_SIGN "^"
#define DOWN_SIGN "v"
#define LEFT_SIGN "<"
#define RIGHT_SIGN ">"

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
    int is_active;
    int move_timer;
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
    int frame_counter = 0;

    int base_move_rate = 10; // jaskółka rusza się co 10 klatek przy speed = 1
    int move_counter = base_move_rate;

    // Konfiguracja spawnowania gwiazd
    const int MAX_STARS = 6;
    // (60 sekund / 6 gwiazd) * FRAME_RATE = 10s * 45 klatek = 450 klatek
    const int STAR_SPAWN_FREQUENCY_FRAMES = (DURATION_SECONDS / MAX_STARS) * FRAME_RATE;
    int star_index_to_spawn = 0; // Którą gwiazdę aktywować następną

    srand(time(NULL));

    // Stars config
    Star stars[6];
    for (int i = 0; i < 6; i++)
    {
        Star star;
        star.is_active = 0;
        star.x = (rand() % (GAME_SCREEN_WIDTH - 1)) + 1;
        star.y = 1;
        star.sign = "*";
        stars[i].move_timer = 5;
        stars[i] = star;
    }

    // Rysowanie jaskółki na początku
    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wrefresh(gameScreen);

    while (total_frames > 0)
    {
        // Odbieranie inputu
        int ch = wgetch(gameScreen);

        if (ch != ERR)
        {
            switch (ch)
            {
            case 'w':
            case 'W':
                swallow->direction = UP;
                swallow->sign = UP_SIGN;
                break;
            case 's':
            case 'S':
                swallow->direction = DOWN;
                swallow->sign = DOWN_SIGN;
                break;
            case 'a':
            case 'A':
                swallow->direction = LEFT;
                swallow->sign = LEFT_SIGN;
                break;
            case 'd':
            case 'D':
                swallow->direction = RIGHT;
                swallow->sign = RIGHT_SIGN;
                break;
            case 'q':
            case 'Q':
                total_frames = 0;
                break;

            // Zmiana prędkości
            case 'o':
            case 'O':
                if (swallow->speed > 1)
                    swallow->speed--;
                break;
            case 'p':
            case 'P':
                if (swallow->speed < 5)
                    swallow->speed++;
                break;

            default:
                break;
            }
        }

        // LOGIKA RUCHU — płynna
        if (move_counter <= 0)
        {
            int prev_y = swallow->y;
            int prev_x = swallow->x;

            // JASKÓŁKA ZAWSZE RUSZA SIĘ O 1 KRATKĘ
            switch (swallow->direction)
            {
            case UP:
                swallow->y--;
                break;
            case DOWN:
                swallow->y++;
                break;
            case LEFT:
                swallow->x--;
                break;
            case RIGHT:
                swallow->x++;
                break;
            }

            // Czyszczenie poprzedniego miejsca
            mvwprintw(gameScreen, prev_y, prev_x, " ");

            // Granice
            if (swallow->y < 1)
                swallow->y = 1;
            if (swallow->y > GAME_SCREEN_HEIGHT - 2)
                swallow->y = GAME_SCREEN_HEIGHT - 2;
            if (swallow->x < 1)
                swallow->x = 1;
            if (swallow->x > GAME_SCREEN_WIDTH - 2)
                swallow->x = GAME_SCREEN_WIDTH - 2;

            // Rysowanie nowej pozycji
            mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);

            // Ustawienie czasu do następnego ruchu zależnie od prędkości
            move_counter = base_move_rate / swallow->speed;
            if (move_counter < 1)
                move_counter = 1;
        }

        if (frame_counter % STAR_SPAWN_FREQUENCY_FRAMES == 0 && star_index_to_spawn < MAX_STARS)
        {
            Star *current_star = &stars[star_index_to_spawn];
            current_star->is_active = 1;
            // Losowanie pozycji X na górnej krawędzi (Y=1)
            current_star->x = (rand() % (GAME_SCREEN_WIDTH - 2)) + 1;
            current_star->y = 1;
            current_star->move_timer = 10;

            star_index_to_spawn++;
        }

        // RUCH I RYSOWANIE GWIAZD
        for (int i = 0; i < MAX_STARS; i++)
        {
            Star *star = &stars[i];

            if (star->is_active)
            {
                // 1. Sprawdzenie, czy nadszedł czas na ruch
                if (star->move_timer <= 0)
                {
                    int prev_y = star->y;

                    // RUCH W DÓŁ
                    star->y++;

                    // Czyszczenie starej pozycji
                    if (prev_y < GAME_SCREEN_HEIGHT - 1)
                    {
                        mvwprintw(gameScreen, prev_y, star->x, " ");
                    }

                    // Reset timera ruchu
                    star->move_timer = 10;
                }
                else
                {
                    // Odejmij 1 od licznika ruchu w każdej klatce
                    star->move_timer--;
                }

                // 2. Sprawdzanie granic i rysowanie
                if (star->y >= GAME_SCREEN_HEIGHT - 1) // Wyszła poza dolną krawędź
                {
                    star->is_active = 0;
                }
                else if (star->is_active)
                {
                    // Rysowanie nowej pozycji (dopóki jest aktywna i w granicach)
                    mvwprintw(gameScreen, star->y, star->x, "%s", star->sign);
                }
            }
        }

        // ODŚWIEŻANIE I SLEEP
        wrefresh(gameScreen);
        usleep(SLEEP_TIME_US);

        total_frames--;
        frame_counter++;
        move_counter--;
    }
}

int main()
{
    // Init
    initscr();
    cbreak();
    noecho();
    curs_set(0);

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