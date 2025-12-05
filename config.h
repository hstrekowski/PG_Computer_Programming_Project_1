#ifndef CONFIG_H
#define CONFIG_H

#include "game.h"

// Wczytuje konfigurację z pliku levelX.txt
int load_level_config(int level, LevelConfig *lvlConfig);

// Wyświetla formularz startowy (Nick i Level)
void show_start_screen(WINDOW *gameScreen, PlayerConfig *config);

#endif