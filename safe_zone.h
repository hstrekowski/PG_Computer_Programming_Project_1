#ifndef SAFE_ZONE_H
#define SAFE_ZONE_H

#include "game.h"
#include "swallow.h"

// Inicjalizacja struktury strefy
void init_safe_zone(SafeZone *sz);

// Obsługa klawisza 'T' i aktywacja strefy
void handle_safe_zone_input(SafeZone *sz, Swallow *swallow, int ch, WINDOW *win);

// Aktualizacja liczników czasu strefy
void update_safe_zone(SafeZone *sz);

// Rysowanie ramki strefy
void draw_safe_zone(WINDOW *win, SafeZone *sz);

#endif