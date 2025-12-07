#include "swallow.h"

// Ustawienie początkowe gracza
void init_swallow(Swallow *swallow)
{
    swallow->x = GAME_SCREEN_WIDTH / 2;
    swallow->y = GAME_SCREEN_HEIGHT - 3;
    swallow->direction = UP;
    swallow->sign = SWALLOW_ANIM_UP_FLAP;
    swallow->speed = SWALLOW_INIT_SPEED;
    swallow->lifeForce = SWALLOW_INIT_LIVES;
    swallow->minSpeedLimit = 1;
    swallow->maxSpeedLimit = 5;
    swallow->anim_ticker = 0;
}

// Przetwarzanie klawiszy ruchu
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

// Pobieranie animowanego znaku
char *get_animated_sign(int direction, int ticker)
{
    int flap = (ticker / (BASE_MOVE_RATE / 2)) % 2;
    switch (direction)
    {
    case UP:
        return flap ? SWALLOW_ANIM_UP_FLAP : SWALLOW_ANIM_UP_GLIDE;
    case DOWN:
        return flap ? SWALLOW_ANIM_DOWN_FLAP : SWALLOW_ANIM_DOWN_GLIDE;
    case LEFT:
        return flap ? SWALLOW_ANIM_LEFT_FLAP : SWALLOW_ANIM_LEFT_GLIDE;
    case RIGHT:
        return flap ? SWALLOW_ANIM_RIGHT_FLAP : SWALLOW_ANIM_RIGHT_GLIDE;
    }
    return SWALLOW_ANIM_UP_FLAP;
}

// Obliczanie następnej pozycji gracza
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
        s->sign = SWALLOW_ANIM_DOWN_FLAP;
    }
    else if (s->y > GAME_SCREEN_HEIGHT - 2)
    {
        s->y = GAME_SCREEN_HEIGHT - 3;
        s->direction = UP;
        s->sign = SWALLOW_ANIM_UP_FLAP;
    }
    else if (s->x < 1)
    {
        s->x = 2;
        s->direction = RIGHT;
        s->sign = SWALLOW_ANIM_RIGHT_FLAP;
    }
    else if (s->x > GAME_SCREEN_WIDTH - 2)
    {
        s->x = GAME_SCREEN_WIDTH - 3;
        s->direction = LEFT;
        s->sign = SWALLOW_ANIM_LEFT_FLAP;
    }
}

// Rysowanie postaci z kolorem
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

// Główna pętla ruchu gracza
void update_swallow_position(WINDOW *gameScreen, Swallow *s, int *move_counter)
{
    s->anim_ticker++;
    if (*move_counter <= 0)
    {
        mvwprintw(gameScreen, s->y, s->x, " ");
        calculate_next_pos(s);
        draw_swallow_visuals(gameScreen, s);

        *move_counter = BASE_MOVE_RATE / s->speed;
        if (*move_counter < 1)
            *move_counter = 1;
    }
    else
    {
        draw_swallow_visuals(gameScreen, s);
    }
}