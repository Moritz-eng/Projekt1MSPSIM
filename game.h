#ifndef GAME_H
#define GAME_H

#include "graphics.h"

typedef struct { //Datenstruktur aber verletzung der single responsebility
  int score; //aktuelle Punkte
  int counter;
  int running; //1 → Spiel läuft 0 → beendet
  Crosshair crosshair;
  Target target;
  int button_prev;
  int restart_prev;
  int sound_timer;
} GameState;

void game_init(GameState* state);

void game_start(GameState* state);
void game_update(GameState* state);
void game_handle_input(GameState* state);

int check_hit(const Crosshair* crosshair, const Target* target);

#endif