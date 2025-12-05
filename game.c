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

void update_status(WINDOW *statusArea, PlayerConfig *config, LevelConfig *lvlConfig, int score, int lifeForce, int seconds, int speed, int starsFumbled)
{
    werase(statusArea);
    box(statusArea, 0, 0);
    mvwprintw(statusArea, 1, 2, "PLAYER: %s", config->name);
    mvwprintw(statusArea, 1, 40, "LEVEL: %d", lvlConfig->levelNumber);

    if (score >= lvlConfig->starGoal)
    {
        wattron(statusArea, COLOR_PAIR(PAIR_HUNTER_GREEN));
        mvwprintw(statusArea, 2, 2, "STARS: %d / %d (GOAL REACHED!)", score, lvlConfig->starGoal);
        wattroff(statusArea, COLOR_PAIR(PAIR_HUNTER_GREEN));
    }
    else
    {
        mvwprintw(statusArea, 2, 2, "STARS: %d / %d", score, lvlConfig->starGoal);
    }

    wattron(statusArea, COLOR_PAIR(lifeForce <= 1 ? PAIR_RED : (lifeForce == 2 ? PAIR_ORANGE : PAIR_WHITE)));
    mvwprintw(statusArea, 2, 40, "LIVES: %d", lifeForce);
    wattroff(statusArea, COLOR_PAIR(lifeForce <= 1 ? PAIR_RED : (lifeForce == 2 ? PAIR_ORANGE : PAIR_WHITE)));
    mvwprintw(statusArea, 2, 70, "TIME: %d s", seconds);
    mvwprintw(statusArea, 3, 2, "FUMBLED: %d", starsFumbled);
    mvwprintw(statusArea, 3, 40, "SPEED: %d", speed);
    mvwhline(statusArea, 4, 1, 0, STATUS_AREA_WIDTH - 2);

    mvwprintw(statusArea, 5, 2, "[ CONTROLS ]");
    mvwprintw(statusArea, 5, 20, "W / S / A / D - Move");
    mvwprintw(statusArea, 5, 55, "T - SAFE ZONE (Teleport)"); // Info dla gracza

    mvwprintw(statusArea, 6, 20, "O / P - Speed Control");
    wattron(statusArea, COLOR_PAIR(PAIR_RED));
    mvwprintw(statusArea, 6, 90, "Q - QUIT GAME");
    wattroff(statusArea, COLOR_PAIR(PAIR_RED));
    wrefresh(statusArea);
}

int load_level_config(int level, LevelConfig *lvlConfig)
{
    char filename[50];
    sprintf(filename, "level%d.txt", level);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;

    // Wartości domyślne
    lvlConfig->levelNumber = level;
    lvlConfig->durationSeconds = 60;
    lvlConfig->maxStars = 50;
    lvlConfig->maxHunters = 5;
    lvlConfig->starFreq = 15;
    lvlConfig->hunterFreq = 100;
    lvlConfig->hunterDamage = 1;
    lvlConfig->minSpeed = 1;
    lvlConfig->maxSpeed = 5;
    lvlConfig->starGoal = 10; // Domyślny cel
    for (int i = 0; i < 5; i++)
        lvlConfig->allowedHunterTypes[i] = 1;

    char key[50];
    while (fscanf(file, "%s", key) != EOF)
    {
        if (strcmp(key, "DURATION") == 0)
            fscanf(file, "%d", &lvlConfig->durationSeconds);
        else if (strcmp(key, "MAX_STARS") == 0)
            fscanf(file, "%d", &lvlConfig->maxStars);
        else if (strcmp(key, "MAX_HUNTERS") == 0)
            fscanf(file, "%d", &lvlConfig->maxHunters);
        else if (strcmp(key, "STAR_FREQ") == 0)
            fscanf(file, "%d", &lvlConfig->starFreq);
        else if (strcmp(key, "HUNTER_FREQ") == 0)
            fscanf(file, "%d", &lvlConfig->hunterFreq);
        else if (strcmp(key, "HUNTER_DAMAGE") == 0)
            fscanf(file, "%d", &lvlConfig->hunterDamage);

        // NOWE: Czytanie celu
        else if (strcmp(key, "STAR_GOAL") == 0)
            fscanf(file, "%d", &lvlConfig->starGoal);

        else if (strcmp(key, "SPEED_LIMITS") == 0)
        {
            fscanf(file, "%d %d", &lvlConfig->minSpeed, &lvlConfig->maxSpeed);
        }
        else if (strcmp(key, "HUNTER_TYPES") == 0)
        {
            for (int i = 0; i < 5; i++)
                fscanf(file, "%d", &lvlConfig->allowedHunterTypes[i]);
        }
    }

    fclose(file);
    return 1;
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
    LevelConfig lvlConfig;
    int success = load_level_config(config->startLevel, &lvlConfig);
    if (!success)
    {
        wclear(gameScreen);
        box(gameScreen, 0, 0);
        mvwprintw(gameScreen, GAME_SCREEN_HEIGHT / 2, 10, "ERROR: Could not load level%d.txt!", config->startLevel);
        wrefresh(gameScreen);
        return;
    }

    swallow->minSpeedLimit = lvlConfig.minSpeed;
    swallow->maxSpeedLimit = lvlConfig.maxSpeed;
    if (swallow->speed < swallow->minSpeedLimit)
        swallow->speed = swallow->minSpeedLimit;
    if (swallow->speed > swallow->maxSpeedLimit)
        swallow->speed = swallow->maxSpeedLimit;

    Stats stats = {0, 0};
    int frames_left = lvlConfig.durationSeconds * FRAME_RATE;
    const int INITIAL_TOTAL_FRAMES = frames_left;
    int frame_counter = 0;
    const int SLEEP_TIME_US = 1000000 / FRAME_RATE;
    int move_counter = BASE_MOVE_RATE;
    int star_index_to_spawn = 0;

    SafeZone safeZone;
    safeZone.is_active = 0;
    safeZone.x = GAME_SCREEN_WIDTH / 2;
    safeZone.y = GAME_SCREEN_HEIGHT - 6;
    safeZone.duration_timer = 0;
    safeZone.cooldown_timer = 0;
    safeZone.game_start_timer = 0;

    srand(time(NULL));

    Star stars[MAX_STARS_LIMIT];
    init_stars(stars);

    Hunter hunters[MAX_HUNTERS_LIMIT];
    init_hunters(hunters);

    wattron(gameScreen, COLOR_PAIR(PAIR_WHITE));
    mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
    wattroff(gameScreen, COLOR_PAIR(PAIR_WHITE));
    wrefresh(gameScreen);

    update_status(statusArea, config, &lvlConfig, stats.score, swallow->lifeForce, frames_left / FRAME_RATE, swallow->speed, stats.starsFumbled);

    while (frames_left > 0)
    {
        // --- OBSŁUGA KLAWISZY ---
        int ch = wgetch(gameScreen);

        // 1. Sprawdzamy aktywację 'T'
        if ((ch == 't' || ch == 'T'))
        {
            if (safeZone.game_start_timer > 5 * FRAME_RATE && safeZone.cooldown_timer <= 0)
            {
                safeZone.is_active = 1;
                safeZone.duration_timer = 5 * FRAME_RATE;
                safeZone.cooldown_timer = 30 * FRAME_RATE;

                // Czyścimy starą pozycję przed teleportem
                mvwprintw(gameScreen, swallow->y, swallow->x, " ");

                // Teleportacja
                swallow->x = safeZone.x;
                swallow->y = safeZone.y;
                swallow->direction = UP;
                swallow->sign = "^";
            }
        }
        // 2. Obsługa reszty klawiszy (tylko jeśli NIE jesteśmy w Safe Zone)
        else if (ch != ERR)
        {
            if (!safeZone.is_active)
            {
                // Pozwalamy sterować tylko gdy strefa NIEAKTYWNA
                ungetch(ch);
                int should_quit = handle_input(gameScreen, swallow);
                if (should_quit)
                    break;
            }
            else
            {
                // Jeśli strefa aktywna, pozwalamy TYLKO na wyjście 'Q'
                if (ch == 'q' || ch == 'Q')
                    break;
            }
        }

        // --- WAŻNE: LOGIKA RUCHU I STREFY ---
        if (safeZone.is_active)
        {
            // SZTYWNE UWIĘZIENIE
            // Nadpisujemy pozycję na środek strefy w każdej klatce
            swallow->x = safeZone.x;
            swallow->y = safeZone.y;

            // RĘCZNE RYSOWANIE JASKÓŁKI (bo update_swallow_position jest wyłączone)
            // Najpierw czyścimy to miejsce (na wypadek artefaktów), potem rysujemy
            // (Chociaż przy stałej pozycji nie trzeba czyścić, ale dla pewności)

            wattron(gameScreen, COLOR_PAIR(PAIR_ORANGE));
            mvwprintw(gameScreen, swallow->y, swallow->x, "%s", swallow->sign);
            wattroff(gameScreen, COLOR_PAIR(PAIR_ORANGE));

            // UWAGA: NIE wywołujemy update_swallow_position!
        }
        else
        {
            // NORMALNY RUCH (tylko gdy strefa nieaktywna)
            update_swallow_position(gameScreen, swallow, &move_counter);
        }
        // ----------------------------------------

        try_spawn_star(stars, &star_index_to_spawn, frame_counter, lvlConfig.starFreq, lvlConfig.maxStars);
        update_stars(gameScreen, stars, swallow, &stats);

        try_spawn_hunter(hunters, swallow, frame_counter, INITIAL_TOTAL_FRAMES, lvlConfig.hunterFreq, lvlConfig.maxHunters, lvlConfig.allowedHunterTypes);
        update_hunters(gameScreen, hunters, swallow, lvlConfig.hunterDamage, &safeZone);

        // --- Rysowanie RAMKI strefy ---
        // --- Rysowanie RAMKI strefy ---
        if (safeZone.is_active)
        {
            safeZone.duration_timer--;

            // JEŚLI CZAS SIĘ SKOŃCZYŁ:
            if (safeZone.duration_timer <= 0)
            {
                safeZone.is_active = 0;

                // --- POPRAWKA: Czyścimy ramkę (zamazujemy spacjami) ---
                int radius = 1;
                for (int dy = -radius; dy <= radius; dy++)
                {
                    for (int dx = -radius; dx <= radius; dx++)
                    {
                        if (abs(dx) == radius || abs(dy) == radius)
                        {
                            mvwprintw(gameScreen, safeZone.y + dy, safeZone.x + dx, " ");
                        }
                    }
                }
                // -----------------------------------------------------
            }
            else
            {
                // JEŚLI NADAL AKTYWNA: Rysujemy ramkę
                int radius = 1;
                wattron(gameScreen, COLOR_PAIR(PAIR_ORANGE));
                for (int dy = -radius; dy <= radius; dy++)
                {
                    for (int dx = -radius; dx <= radius; dx++)
                    {
                        if (abs(dx) == radius || abs(dy) == radius)
                        {
                            int d_y = safeZone.y + dy;
                            int d_x = safeZone.x + dx;
                            // Sprawdzamy granice, żeby nie rysować po ramce okna
                            if (d_y > 0 && d_y < GAME_SCREEN_HEIGHT - 1 && d_x > 0 && d_x < GAME_SCREEN_WIDTH - 1)
                                mvwprintw(gameScreen, d_y, d_x, "O");
                        }
                    }
                }
                wattroff(gameScreen, COLOR_PAIR(PAIR_ORANGE));
            }
        }

        safeZone.game_start_timer++;
        if (safeZone.cooldown_timer > 0)
            safeZone.cooldown_timer--;

        if (swallow->lifeForce <= 0)
            break;

        update_status(statusArea, config, &lvlConfig, stats.score, swallow->lifeForce, frames_left / FRAME_RATE, swallow->speed, stats.starsFumbled);

        // Status Safe Zone
        if (safeZone.is_active)
        {
            wattron(statusArea, COLOR_PAIR(PAIR_HUNTER_BLUE));
            mvwprintw(statusArea, 5, 85, "ZONE: ACTIVE %ds ", (safeZone.duration_timer / FRAME_RATE) + 1);
            wattroff(statusArea, COLOR_PAIR(PAIR_HUNTER_BLUE));
        }
        else if (safeZone.cooldown_timer > 0)
        {
            mvwprintw(statusArea, 5, 85, "ZONE: WAIT %ds  ", (safeZone.cooldown_timer / FRAME_RATE) + 1);
        }
        else if (safeZone.game_start_timer > 5 * FRAME_RATE)
        {
            wattron(statusArea, COLOR_PAIR(PAIR_HUNTER_GREEN));
            mvwprintw(statusArea, 5, 85, "ZONE: READY!    ");
            wattroff(statusArea, COLOR_PAIR(PAIR_HUNTER_GREEN));
        }
        else
        {
            mvwprintw(statusArea, 5, 85, "ZONE: LOCKED    ");
        }
        wrefresh(statusArea);

        wrefresh(gameScreen);
        usleep(SLEEP_TIME_US);
        frames_left--;
        frame_counter++;
        move_counter--;
    }

    // GAME OVER SCREEN (Bez zmian)
    wclear(gameScreen);
    box(gameScreen, 0, 0);
    char msg_result[50];
    if (swallow->lifeForce > 0 && stats.score >= lvlConfig.starGoal)
        strcpy(msg_result, "LEVEL COMPLETED!");
    else if (swallow->lifeForce <= 0)
        strcpy(msg_result, "GAME OVER - You Died!");
    else
        strcpy(msg_result, "GAME OVER - Time's Up!");
    char msg_stats[150];
    int final_lives = swallow->lifeForce < 0 ? 0 : swallow->lifeForce;
    sprintf(msg_stats, "Player: %s | Stars: %d/%d | Lives: %d", config->name, stats.score, lvlConfig.starGoal, final_lives);
    int center_y = GAME_SCREEN_HEIGHT / 2;
    int center_x = GAME_SCREEN_WIDTH / 2;
    mvwprintw(gameScreen, center_y - 2, center_x - (strlen(msg_result) / 2), "%s", msg_result);
    mvwprintw(gameScreen, center_y, center_x - (strlen(msg_stats) / 2), "%s", msg_stats);
    wrefresh(gameScreen);
}