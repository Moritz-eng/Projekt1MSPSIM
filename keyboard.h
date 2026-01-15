#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Tastatur-Zustand
typedef struct {
    int cursor_x;
    int cursor_y;
    char input[16];
    int input_pos;
    int done;
    int input_changed;
} KeyboardState;

// Tastatur-Funktionen
void keyboard_init(KeyboardState* kb);
void keyboard_draw(void);
void keyboard_update(KeyboardState* kb);
void keyboard_handle_input(KeyboardState* kb);

char keyboard_get_char(int x, int y);

// Zeichenfunktionen für inkrementelles Redraw
void draw_key_normal(int col, int row);
void draw_key_selected(int col, int row);
void keyboard_update_cursor(KeyboardState* kb, int old_x, int old_y);

#endif // KEYBOARD_H
