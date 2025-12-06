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
        return 1;
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
    int flap = (ticker / 5) % 2;
    switch (direction)
    {
    case UP:
        return flap ? "^" : "-";
    case DOWN:
        return flap ? "v" : "W";
    case LEFT:
        return flap ? "<" : "{";
    case RIGHT:
        return flap ? ">" : "}";
    }
    return "^";
}

// Funkcja pomocnicza: Oblicza pozycję i odbicia
static void calculate_next_pos(Swallow *s)
{
    switch (s->direction)
    {
    case UP:
        s->y--;
        break;
    case DOWN:
        s->y++;
        break;
    case LEFT:
        s->x--;
        break;
    case RIGHT:
        s->x++;
        break;
    }
    if (s->y < 1)
    {
        s->y = 2;
        s->direction = DOWN;
        s->sign = DOWN_SIGN;
    }
    else if (s->y > GAME_SCREEN_HEIGHT - 2)
    {
        s->y = GAME_SCREEN_HEIGHT - 3;
        s->direction = UP;
        s->sign = UP_SIGN;
    }
    else if (s->x < 1)
    {
        s->x = 2;
        s->direction = RIGHT;
        s->sign = RIGHT_SIGN;
    }
    else if (s->x > GAME_SCREEN_WIDTH - 2)
    {
        s->x = GAME_SCREEN_WIDTH - 3;
        s->direction = LEFT;
        s->sign = LEFT_SIGN;
    }
}

// Funkcja pomocnicza: Rysowanie
static void draw_swallow_visuals(WINDOW *win, Swallow *s)
{
    s->sign = get_animated_sign(s->direction, s->anim_ticker);
    int color = PAIR_WHITE;
    if (s->lifeForce == 2)
        color = PAIR_ORANGE;
    else if (s->lifeForce <= 1)
        color = PAIR_RED;

    wattron(win, COLOR_PAIR(color));
    mvwprintw(win, s->y, s->x, "%s", s->sign);
    wattroff(win, COLOR_PAIR(color));
}

void update_swallow_position(WINDOW *gameScreen, Swallow *s, int *move_counter)
{
    s->anim_ticker++;
    if (*move_counter <= 0)
    {
        mvwprintw(gameScreen, s->y, s->x, " "); // Wyczyść stare
        calculate_next_pos(s);
        draw_swallow_visuals(gameScreen, s);

        *move_counter = BASE_MOVE_RATE / s->speed;
        if (*move_counter < 1)
            *move_counter = 1;
    }
    else
    {
        // Animacja w miejscu
        draw_swallow_visuals(gameScreen, s);
    }
}