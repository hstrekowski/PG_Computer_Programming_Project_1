#include "swallow.h"

void init_swallow(Swallow *swallow)
{
    swallow->x = GAME_SCREEN_WIDTH / 2;
    swallow->y = GAME_SCREEN_HEIGHT - 3;
    swallow->direction = UP;
    swallow->sign = UP_SIGN;
    swallow->speed = 1;
    swallow->lifeForce = 3;
    swallow->minSpeedLimit = 1;
    swallow->maxSpeedLimit = 5;
    swallow->anim_ticker = 0;
}

int handle_input(Swallow *swallow, int ch)
{
    // Nie robimy tutaj wgetch!

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
        return 1; // Sygnał wyjścia
    case 'o':
    case 'O':
        if (swallow->speed > swallow->minSpeedLimit)
            swallow->speed--;
        break;
    case 'p':
    case 'P':
        if (swallow->speed < swallow->maxSpeedLimit)
            swallow->speed++;
        break;
    }
    return 0;
}

char *get_animated_sign(int direction, int ticker)
{
    // Machamy co 5 klatek
    int flap = (ticker / 5) % 2;

    switch (direction)
    {
    // Góra: ^ (złożone) vs - (rozłożone)
    case UP:
        return flap ? "^" : "-";
    // Dół: v (złożone) vs W (rozłożone)
    case DOWN:
        return flap ? "v" : "W";
    // Lewo: < vs {
    case LEFT:
        return flap ? "<" : "{";
    // Prawo: > vs }
    case RIGHT:
        return flap ? ">" : "}";
    }
    return "^";
}

void update_swallow_position(WINDOW *gameScreen, Swallow *swallow, int *move_counter)
{
    swallow->anim_ticker++;

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

        // Górna ściana
        if (swallow->y < 1)
        {
            swallow->y = 2; // Odbijamy na pozycję 2
            swallow->direction = DOWN;
            swallow->sign = DOWN_SIGN;
        }
        // Dolna ściana
        else if (swallow->y > GAME_SCREEN_HEIGHT - 2)
        {
            swallow->y = GAME_SCREEN_HEIGHT - 3; // Odbijamy w górę
            swallow->direction = UP;
            swallow->sign = UP_SIGN;
        }
        // Lewa ściana
        else if (swallow->x < 1)
        {
            swallow->x = 2; // Odbijamy w prawo
            swallow->direction = RIGHT;
            swallow->sign = RIGHT_SIGN;
        }
        // Prawa ściana
        else if (swallow->x > GAME_SCREEN_WIDTH - 2)
        {
            swallow->x = GAME_SCREEN_WIDTH - 3; // Odbijamy w lewo
            swallow->direction = LEFT;
            swallow->sign = LEFT_SIGN;
        }

        // --- Rysowanie ---
        mvwprintw(gameScreen, prev_y, prev_x, " ");

        // ZMIANA: Dobór koloru
        int color_pair = PAIR_WHITE; // Domyślny (3 życia lub więcej)
        if (swallow->lifeForce == 2)
        {
            color_pair = PAIR_ORANGE;
        }
        else if (swallow->lifeForce <= 1)
        {
            color_pair = PAIR_RED;
        }

        swallow->sign = get_animated_sign(swallow->direction, swallow->anim_ticker);

        // Włącz kolor -> Rysuj -> Wyłącz kolor
        wattron(gameScreen, COLOR_PAIR(color_pair));
        mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
        wattroff(gameScreen, COLOR_PAIR(color_pair));

        *move_counter = BASE_MOVE_RATE / swallow->speed;
        if (*move_counter < 1)
            *move_counter = 1;
    }
    else
    {
        // Jeśli się nie ruszamy, ale chcemy animować w miejscu (machanie skrzydłami w locie)
        // musimy przerysować jaskółkę w tej samej pozycji
        swallow->sign = get_animated_sign(swallow->direction, swallow->anim_ticker);

        int color_pair = PAIR_WHITE;
        if (swallow->lifeForce == 2)
            color_pair = PAIR_ORANGE;
        else if (swallow->lifeForce <= 1)
            color_pair = PAIR_RED;

        wattron(gameScreen, COLOR_PAIR(color_pair));
        mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
        wattroff(gameScreen, COLOR_PAIR(color_pair));
    }
}