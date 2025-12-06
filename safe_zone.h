#ifndef SAFE_ZONE_H
#define SAFE_ZONE_H

#include "game.h"
#include "swallow.h"

// Inicjalizacja
void init_safe_zone(SafeZone *sz);

// Input
void handle_safe_zone_input(SafeZone *sz, Swallow *swallow, int ch, WINDOW *win);

// Update
void update_safe_zone(SafeZone *sz);

// Render
void draw_safe_zone(WINDOW *win, SafeZone *sz);

// NOWE: Funkcja efektu mrugania (publiczna dla replayu)
void blink_effect(WINDOW *win);

#endif