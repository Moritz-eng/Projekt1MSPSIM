#include <stdio.h>
#include <string.h>
#include "ST7735.h"
#include "config.h"
#include "graphics.h"
#include "input.h"
#include "keyboard.h"

static const char* keyboard_layout[] = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ"};

char last_used_name[16] = "";

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
  if (y == 2 && x >= 6) return 0;
  if (x < 0 || x >= 10) return 0;
  char c = keyboard_layout[y][x];
  if (c == ' ') return 0;
  return c;
}

void keyboard_draw(void) {
  setText(10, 5, "ENTER NAME:", COLOR_BLACK, COLOR_WHITE);

  int x = KEY_START_X;
  int y = KEY_START_Y + 3 * KEY_HEIGHT;
  draw(x, y, DISPLAY_WIDTH - 8, KEY_HEIGHT - 2, COLOR_WHITE);
  if (last_used_name[0] != '\0') {
    char buf[32];
    sprintf(buf, "Last Name");
    setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
  } else {
    setText(x + 3, y + 2, "No Name", COLOR_BLACK, COLOR_WHITE);
  }

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 10; col++) {
      int x = KEY_START_X + col * KEY_WIDTH;
      int y = KEY_START_Y + row * KEY_HEIGHT;

      if (row == 2) {
        if (col == 6) {
          draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, COLOR_RED);
          setText(x + 1, y + 2, "DEL", COLOR_WHITE, COLOR_RED);
        } else if (col == 8) {
          draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0x00FF00);
          setText(x + 3, y + 2, "OK", COLOR_BLACK, 0x00FF00);
        } else if (col > 5) {
          continue;
        } else {
          char c = keyboard_get_char(col, row);
          if (c) {
            draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
            char buf[2] = {c, '\0'};
            setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
          }
        }
        continue;
      }

      char c = keyboard_get_char(col, row);
      if (c) {
        draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
        char buf[2] = {c, '\0'};
        setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
      }
    }
  }
}

void draw_key_normal(int col, int row) {
  int x = KEY_START_X + col * KEY_WIDTH;
  int y = KEY_START_Y + row * KEY_HEIGHT;

  if (row == 3) {
    draw(KEY_START_X, y, DISPLAY_WIDTH - 8, (KEY_HEIGHT * 2) - 2, COLOR_WHITE);
    setText(KEY_START_X + 5, y + 2, "Last Name:", COLOR_BLACK, COLOR_WHITE);
    setText(KEY_START_X + 5, y + 14, last_used_name, COLOR_RED, COLOR_WHITE);
    return;
  }

  if (row == 2) {
    if (col == 6) {
      draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, COLOR_RED);
      setText(x + 1, y + 2, "DEL", COLOR_WHITE, COLOR_RED);
      return;
    } else if (col == 8) {
      draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, 0x00FF00);
      setText(x + 3, y + 2, "OK", COLOR_BLACK, 0x00FF00);
      return;
    } else if (col > 5)
      return;
  }

  char c = keyboard_get_char(col, row);
  if (c) {
    draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_WHITE);
    char buf[2] = {c, '\0'};
    setText(x + 3, y + 2, buf, COLOR_BLACK, COLOR_WHITE);
  }
}

void draw_key_selected(int col, int row) {
  int x = KEY_START_X + col * KEY_WIDTH;
  int y = KEY_START_Y + row * KEY_HEIGHT;

  if (row == 3) {
    draw(KEY_START_X, y, DISPLAY_WIDTH - 8, (KEY_HEIGHT * 2) - 2, COLOR_BLUE);
    setText(KEY_START_X + 5, y + 2, "Last Name:", COLOR_WHITE, COLOR_BLUE);
    setText(KEY_START_X + 5, y + 14, last_used_name, COLOR_WHITE, COLOR_BLUE);
    return;
  }

  if (row == 2) {
    if (col == 6) {
      draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, COLOR_BLUE);
      setText(x + 1, y + 2, "DEL", COLOR_BLACK, COLOR_BLUE);
      return;
    } else if (col == 8) {
      draw(x, y, KEY_WIDTH * 2 - 2, KEY_HEIGHT - 2, COLOR_BLUE);
      setText(x + 3, y + 2, "OK", COLOR_BLACK, COLOR_BLUE);
      return;
    } else if (col > 5)
      return;
  }

  char c = keyboard_get_char(col, row);
  if (c) {
    draw(x, y, KEY_WIDTH - 2, KEY_HEIGHT - 2, COLOR_BLUE);
    char buf[2] = {c, '\0'};
    setText(x + 3, y + 2, buf, COLOR_WHITE, COLOR_BLUE);
  }
}

void keyboard_update_cursor(KeyboardState* kb, int old_x, int old_y) {
  if (old_x >= 0 && old_y >= 0) draw_key_normal(old_x, old_y);

  draw_key_selected(kb->cursor_x, kb->cursor_y);
}

void keyboard_update(KeyboardState* kb) {
  static int old_x = -1, old_y = -1;

  if (kb->cursor_x != old_x || kb->cursor_y != old_y) {
    keyboard_update_cursor(kb, old_x, old_y);
    old_x = kb->cursor_x;
    old_y = kb->cursor_y;
  }
  if (kb->input_changed) {
    draw(10, 20, 110, 12, COLOR_WHITE);
    if (kb->input_pos > 0) setText(10, 20, kb->input, COLOR_BLACK, COLOR_WHITE);
    kb->input_changed = 0;
  }
}

void keyboard_handle_input(KeyboardState* kb) {
  static Crosshair joy_state = {64, 64, 64, 64};
  static int button_prev = 0;

  joystick_update(&joy_state, 3);

  int dx = joy_state.x - joy_state.old_x;
  int dy = joy_state.y - joy_state.old_y;

  if (dx > 1) {
    kb->cursor_x++;
    if (kb->cursor_y == 2) {
      if (kb->cursor_x == 7) kb->cursor_x = 8;
      if (kb->cursor_x > 8) kb->cursor_x = 8;
    } else if (kb->cursor_y == 3) {
      kb->cursor_x = 0;
    } else if (kb->cursor_x > 9) {
      kb->cursor_x = 9;
    }
    joy_state.old_x = 64;
    joy_state.x = 64;
  } else if (dx < -1) {
    kb->cursor_x--;
    if (kb->cursor_y == 2) {
      if (kb->cursor_x == 7) kb->cursor_x = 6;
      if (kb->cursor_x < 0) kb->cursor_x = 0;
    } else if (kb->cursor_y == 3) {
      kb->cursor_x = 0;
    } else if (kb->cursor_x < 0) {
      kb->cursor_x = 0;
    }
    joy_state.old_x = 64;
    joy_state.x = 64;
  }

  if (dy > 1) {
    kb->cursor_y++;
    if (kb->cursor_y > 3) kb->cursor_y = 3;

    if (kb->cursor_y == 2 && kb->cursor_x > 8) kb->cursor_x = 8;
    if (kb->cursor_y == 3) kb->cursor_x = 0;

    joy_state.old_y = 64;
    joy_state.y = 64;
  } else if (dy < -1) {
    kb->cursor_y--;
    if (kb->cursor_y < 0) kb->cursor_y = 0;

    if (kb->cursor_y < 3 && kb->cursor_x == 0 && dx == 0) kb->cursor_x = 4;

    joy_state.old_y = 64;
    joy_state.y = 64;
  }

  int pressed = button_pressed();
  if (pressed && !button_prev) {
    if (kb->cursor_y == 3) {
      if (last_used_name[0] != '\0') {
        strncpy(kb->input, last_used_name, 15);
        kb->input_pos = strlen(kb->input);
        kb->input_changed = 1;
      }
    } else if (kb->cursor_y == 2 && kb->cursor_x == 8) {
      if (kb->input_pos > 0) {
        strncpy(last_used_name, kb->input, 15);
        kb->done = 1;
      }
    } else if (kb->cursor_y == 2 && kb->cursor_x == 6) {
      if (kb->input_pos > 0) {
        kb->input_pos--;
        kb->input[kb->input_pos] = '\0';
        kb->input_changed = 1;
      }
    } else {
      char c = keyboard_get_char(kb->cursor_x, kb->cursor_y);
      if (c && kb->input_pos < 8) {
        kb->input[kb->input_pos++] = c;
        kb->input[kb->input_pos] = '\0';
        kb->input_changed = 1;
      }
    }
  }

  button_prev = pressed;
}