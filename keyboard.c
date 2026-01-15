#include "keyboard.h"
#include "config.h"
#include "graphics.h"
#include "input.h"
#include "ST7735.h"
#include <string.h>

// Tastatur-Layout (3 Zeilen)
static const char* keyboard_layout[] = {
    "ABCDEFGHIJ",   // Zeile 0
    "KLMNOPQRST",   // Zeile 1
    "UVWXYZ"        // Zeile 2, Spalten 0–5, Spalten 6–9 für DEL/OK
};

static const int KEY_WIDTH = 12;
static const int KEY_HEIGHT = 15;
static const int KEY_START_Y = 40;
static const int KEY_START_X = 4;

void keyboard_init(KeyboardState* kb) {
    kb->cursor_x = 0;
    kb->cursor_y = 0;
    kb->input_pos = 0;
    kb->done = 0;
    kb->input_changed = 1;
    memset(kb->input, 0, sizeof(kb->input));
}

char keyboard_get_char(int x, int y) {
    if (y < 0 || y >= 3) return 0;
    if (y == 2 && x >= 6) return 0; // Zeile 2, Spalten 6-9 → keine Buchstaben
    if (x < 0 || x >= 10) return 0;
    char c = keyboard_layout[y][x];
    if (c == ' ') return 0;
    return c;
}

// Zeichnet die komplette Tastatur (einmalig)
void keyboard_draw(void) {
    setText(10, 5, "ENTER NAME:", COLOR_BLACK, COLOR_WHITE);

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 10; col++) {
            int x = KEY_START_X + col * KEY_WIDTH;
            int y = KEY_START_Y + row * KEY_HEIGHT;

            if (row == 2) {
                if (col == 6) { // DEL
                    draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0xFF0000);
                    setText(x + 1, y + 2, "DEL", COLOR_WHITE, 0xFF0000);
                } else if (col == 8) { // OK
                    draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0x00FF00);
                    setText(x + 3, y + 2, "OK", COLOR_BLACK, 0x00FF00);
                } else if (col > 5) {
                    continue; // leere Spalten ignorieren
                } else {
                    // Buchstaben U-Z
                    char c = keyboard_get_char(col, row);
                    if (c) {
                        draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
                        char buf[2] = {c, '\0'};
                        setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
                    }
                }
                continue;
            }

            // Zeilen 0 und 1 (normale Buchstaben)
            char c = keyboard_get_char(col, row);
            if (c) {
                draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
                char buf[2] = {c, '\0'};
                setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
            }
        }
    }
}

// Zeichnet normale Taste (nicht selektiert)
void draw_key_normal(int col, int row) {
    int x = KEY_START_X + col * KEY_WIDTH;
    int y = KEY_START_Y + row * KEY_HEIGHT;

    if (row == 2) {
        if (col == 6) {
            draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0xFF0000);
            setText(x + 1, y + 2, "DEL", COLOR_WHITE, 0xFF0000);
            return;
        } else if (col == 8) {
            draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0x00FF00);
            setText(x + 3, y + 2, "OK", COLOR_BLACK, 0x00FF00);
            return;
        } else if (col > 5) return; // leere Spalten
    }

    char c = keyboard_get_char(col, row);
    if (c) {
        draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
        char buf[2] = {c, '\0'};
        setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
    }
}

// Zeichnet selektierte Taste
void draw_key_selected(int col, int row) {
    int x = KEY_START_X + col * KEY_WIDTH;
    int y = KEY_START_Y + row * KEY_HEIGHT;

    if (row == 2) {
        if (col == 6) {
            draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0xFFFF00);
            setText(x + 1, y + 2, "DEL", COLOR_BLACK, 0xFFFF00);
            return;
        } else if (col == 8) {
            draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0xFFFF00);
            setText(x + 3, y + 2, "OK", COLOR_BLACK, 0xFFFF00);
            return;
        } else if (col > 5) return; // leere Spalten
    }

    char c = keyboard_get_char(col, row);
    if (c) {
        draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_BLUE);
        char buf[2] = {c, '\0'};
        setText(x + 3, y + 2, buf, COLOR_WHITE, COLOR_BLUE);
    }
}

// Aktualisiert Cursor (nur alte + neue Taste)
void keyboard_update_cursor(KeyboardState* kb, int old_x, int old_y) {
    if (old_x >= 0 && old_y >= 0)
        draw_key_normal(old_x, old_y);

    draw_key_selected(kb->cursor_x, kb->cursor_y);
}

// Tastaturanzeige aktualisieren
void keyboard_update(KeyboardState* kb) {
    static int old_x = -1, old_y = -1;

    if (kb->cursor_x != old_x || kb->cursor_y != old_y) {
        keyboard_update_cursor(kb, old_x, old_y);
        old_x = kb->cursor_x;
        old_y = kb->cursor_y;
    }
    if(kb->input_changed){
        draw(10, 20, 110, 12, COLOR_WHITE);
        if (kb->input_pos > 0)
            setText(10, 20, kb->input, COLOR_BLACK, COLOR_WHITE);
        kb->input_changed = 0;
    }
}

// Eingaben verarbeiten
void keyboard_handle_input(KeyboardState* kb) {
    static Crosshair joy_state = {64,64,64,64};
    static int button_prev = 0;

    joystick_update(&joy_state, 3);

    int dx = joy_state.x - joy_state.old_x;
    int dy = joy_state.y - joy_state.old_y;

    // Horizontal
    if (dx > 1) {
        kb->cursor_x++;
        if (kb->cursor_y == 2) {
            if (kb->cursor_x == 6) kb->cursor_x = 6; // DEL
            if (kb->cursor_x == 7) kb->cursor_x = 8; // Spring über leere Spalte
            if (kb->cursor_x > 8) kb->cursor_x = 8; // Max OK
        } else if (kb->cursor_x > 9) kb->cursor_x = 9;
        joy_state.old_x = 64;
        joy_state.x = 64;
    } else if (dx < -1) {
        kb->cursor_x--;
        if (kb->cursor_y == 2) {
            if (kb->cursor_x == 7) kb->cursor_x = 6; // Spring über leere Spalte
            if (kb->cursor_x < 0) kb->cursor_x = 0;
        } else if (kb->cursor_x < 0) kb->cursor_x = 0;
        joy_state.old_x = 64;
        joy_state.x = 64;
    }

    // Vertikal
    if (dy > 1) {
        kb->cursor_y++;
        if (kb->cursor_y > 2) kb->cursor_y = 2;
        if (kb->cursor_y == 2 && kb->cursor_x > 8) kb->cursor_x = 8;
        joy_state.old_y = 64;
        joy_state.y =64;
    } else if (dy < -1) {
        kb->cursor_y--;
        if (kb->cursor_y < 0) kb->cursor_y = 0;
        joy_state.old_y = 64;
        joy_state.y =64;
    }

    // Button (Flankenerkennung)
    int pressed = button_pressed();
    if (pressed && !button_prev) {
        if (kb->cursor_y == 2 && kb->cursor_x == 8) {
            kb->done = 1; // OK
        } else if (kb->cursor_y == 2 && kb->cursor_x == 6) {
            if (kb->input_pos > 0) { // DEL
                kb->input_pos--;
                kb->input[kb->input_pos] = '\0';
                kb->input_changed = 1;
            }
        } else {
            char c = keyboard_get_char(kb->cursor_x, kb->cursor_y);
            if (c && kb->input_pos < 15) { // Buchstaben A-Z
                kb->input[kb->input_pos++] = c;
                kb->input[kb->input_pos] = '\0';
                kb->input_changed = 1;
            }
        }
    }

    button_prev = pressed;
}
