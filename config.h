#ifndef CONFIG_H
#define CONFIG_H

#include "game.h"

// Wczytywanie konfiguracji poziomu
int load_level_config(int level, LevelConfig *lvlConfig);

// Wyświetlanie ekranu startowego
void show_start_screen(WINDOW *gameScreen, PlayerConfig *config);

#endif