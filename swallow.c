#include "swallow.h"

void init_swallow(Swallow *swallow)
{
    swallow->x = GAME_SCREEN_WIDTH / 2;
    swallow->y = GAME_SCREEN_HEIGHT - 3;
    swallow->direction = UP;
    swallow->sign = UP_SIGN;
    swallow->speed = 1;
    swallow->lifeForce = 100;
}

int handle_input(WINDOW *gameScreen, Swallow *swallow)
{
    int ch = wgetch(gameScreen);
    if (ch == ERR)
        return 0;

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
        return 1;
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
    }
    return 0;
}

void update_swallow_position(WINDOW *gameScreen, Swallow *swallow, int *move_counter)
{
    if (*move_counter <= 0)
    {
        int prev_y = swallow->y;
        int prev_x = swallow->x;

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

        // Reset licznika ruchu
        *move_counter = BASE_MOVE_RATE / swallow->speed;
        if (*move_counter < 1)
            *move_counter = 1;
    }

    // Dekrementacja licznika w głównej pętli, ale logika resetu jest tutaj
}