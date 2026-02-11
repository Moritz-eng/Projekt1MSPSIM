#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

typedef struct {
  int cursor_x;
  int cursor_y;
  char input[16];
  int input_pos;
  int done;
  int input_changed;
  int on_last_name;
} KeyboardState;
extern char last_used_name[16];

void keyboard_init(KeyboardState* kb);
void keyboard_draw(void);
void keyboard_update(KeyboardState* kb);
void keyboard_handle_input(KeyboardState* kb);

char keyboard_get_char(int x, int y);

void draw_key_normal(int col, int row);
void draw_key_selected(int col, int row);
void keyboard_update_cursor(KeyboardState* kb, int old_x, int old_y);

#endif
