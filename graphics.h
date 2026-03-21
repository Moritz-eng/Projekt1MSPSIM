#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "ST7735.h"
#include <stdint.h>

typedef struct {
  int x;
  int y;
  int vx;
  int vy;
  int half_size;
  int alive;
} Target;

typedef struct {
  int x;
  int y;
  int old_x;
  int old_y;
} Crosshair;

void graphics_init(void);

void draw_cross(int x, int y, uint32_t color);
void draw_square(int cx, int cy, int half_size, uint32_t color);
void erase_cross(int x, int y, const Target* target);
void display_status(int score, int counter);

void spawn_target(Target* target);
void update_target(Target* target);

void clear_screen(void);
void draw_ui_separator(void);

#endif