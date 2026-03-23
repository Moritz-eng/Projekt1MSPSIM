#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "ST7735.h"
#include "audio.h"
#include "config.h"
#include "game.h"
#include "graphics.h"
#include "highscore.h"
#include "input.h"
#include "keyboard.h"

void show_instruction_screen(void) {
  clear_screen();

  setText(15, 10, "INSTRUCTIONS", COLOR_RED, COLOR_WHITE);
  draw(15, 22, 70, 1, COLOR_BLACK);

  setText(5, 40, "- MOVE", COLOR_BLACK, COLOR_WHITE);
  setText(15, 52, "Joystick", COLOR_BLUE, COLOR_WHITE);

  setText(5, 70, "- SHOOT/OK:", COLOR_BLACK, COLOR_WHITE);
  setText(15, 82, "R. LOWER BUTTON", COLOR_BLUE, COLOR_WHITE);

  setText(5, 100, "- Reset:", COLOR_BLACK, COLOR_WHITE);
  setText(15, 112, "R. UP BUTTON", COLOR_BLUE, COLOR_WHITE);

  while (!button_pressed());
  while (button_pressed());
  __delay_cycles(100000);
}

void show_start_screen(char* name) {
  clear_screen();

  char buf[32];
  sprintf(buf, "Hello %s!", name);
  setText(20, 30, buf, COLOR_RED, COLOR_WHITE);

  setText(10, 60, "Goal: Shoot the", COLOR_BLACK, COLOR_WHITE);
  setText(10, 72, "blue Target.", COLOR_BLACK, COLOR_WHITE);

  setText(10, 95, "PRESS OK", COLOR_BLUE, COLOR_WHITE);
  setText(10, 107, "TO START!", COLOR_BLUE, COLOR_WHITE);
}

int main(void) {
  WDTCTL = WDTPW | WDTHOLD;

  graphics_init();
  input_init();
  audio_init();

  UCSCTL4 |= SELA_2;
  TA0CCR0 = 655; // Zählwert (32kHz → 655 → ca. 30ms
  TA0CTL = TASSEL_1 | MC_1 | TACLR; // ACLK (32kHz), Aufwärtszählen

  show_instruction_screen();

  while (1) {
    KeyboardState keyboard;
    keyboard_init(&keyboard);

    clear_screen();
    keyboard_draw();
    draw_key_selected(0, 0);

    while (!keyboard.done) {
      keyboard_handle_input(&keyboard);
      keyboard_update(&keyboard);
      __delay_cycles(200000);
    }

    if (keyboard.input[0] != '\0') {
      strncpy(last_used_name, keyboard.input, 15);
    }

    show_start_screen(keyboard.input);

    while (!button_pressed()) {
    }

    while (button_pressed());
    __delay_cycles(100000);

    clear_screen();

    GameState game;
    game_init(&game);
    game_start(&game); //running auf 1

    while (game.running) {
      while (!(TA0CTL & TAIFG));  // Warten auf Timer-Überlauf  CPU wartet UNTÄTIG!
      TA0CTL &= ~TAIFG;           // Flag löschen
      // Hier passiert das Spiel pro Frame
      draw_ui_separator();
      game_handle_input(&game);
      game_update(&game);
      display_status(game.score, game.counter);
    }
    buzzer_stop();

    highscore_add(keyboard.input, game.score);

    highscore_show();

    while (!restart_button_pressed()) {
    }

    while (restart_button_pressed());
    __delay_cycles(100000);
  }
}