#ifndef INPUT_H
#define INPUT_H

#include "graphics.h"

void input_init(void);

int button_pressed(void);
int restart_button_pressed(void);

void joystick_update(Crosshair* crosshair, int max_speed);

#endif