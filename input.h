#ifndef INPUT_H
#define INPUT_H

#include "graphics.h"

// Initialisierung
void input_init(void);

// Button-Funktionen
int button_pressed(void);
int restart_button_pressed(void);

// Joystick-Funktionen
void joystick_update(Crosshair* crosshair, int max_speed);

#endif // INPUT_H