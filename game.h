#ifndef GAME_H
#define GAME_H

#include "graphics.h"

// Spiel-Zustand
typedef struct {
    int score;
    int counter;
    int running;
    Crosshair crosshair;
    Target target;
    int button_prev;
    int restart_prev;
    int sound_timer;
} GameState;

// Initialisierung
void game_init(GameState* state);

// Spiel-Steuerung
void game_start(GameState* state);
void game_update(GameState* state);
void game_handle_input(GameState* state);

// Kollisionserkennung
int check_hit(const Crosshair* crosshair, const Target* target);

#endif // GAME_H