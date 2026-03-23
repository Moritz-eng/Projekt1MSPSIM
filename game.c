#include "game.h"

#include "audio.h"
#include "config.h"
#include "graphics.h"
#include "input.h"

//Setzt Default-Zustand Spiel startet NICHT automatisch
void game_init(GameState* state) {
  state->score = 0;
  state->counter = 0;
  state->running = 0;
  state->button_prev = 1;
  state->restart_prev = 1;
  //Crosshair zentrieren:
  state->crosshair.x = DISPLAY_WIDTH / 2;
  state->crosshair.y = DISPLAY_HEIGHT / 2;
  state->crosshair.old_x = state->crosshair.x;
  state->crosshair.old_y = state->crosshair.y;
  //Target vorbereiten:
  state->target.half_size = TARGET_HALF_SIZE;
  state->target.alive = 1;
  state->sound_timer = 0;
}
//Reset + Start
void game_start(GameState* state) {
  state->score = 0;
  state->counter = GAME_START_VALUE;
  state->running = 1;

  state->crosshair.x = DISPLAY_WIDTH / 2;
  state->crosshair.y = DISPLAY_HEIGHT / 2;
  state->crosshair.old_x = state->crosshair.x;
  state->crosshair.old_y = state->crosshair.y;
  //UI element fester „Restart“-Button oben rechts
  setText(78, 1, "Restart", COLOR_RED, COLOR_WHITE);
  //Initiales Rendering
  spawn_target(&state->target);
  draw_cross(state->crosshair.x, state->crosshair.y, COLOR_RED);
}
//Wird jede Frame/Iteration aufgerufen in main.c solange running =1
void game_update(GameState* state) {
  //Early Exit
  if (!state->running) return;
  //kurzer Sound nach Schuss
  if (state->sound_timer > 0) {
    state->sound_timer--;
    if (state->sound_timer == 0) buzzer_stop();
  }
  //Countdown → Game Over
  if (state->counter > 0) {
    state->counter--;
  } else {
    state->running = 0;
    buzzer_stop();
    return;
  }
  //Input (Joystick)
  joystick_update(&state->crosshair, MAX_SPEED);
  //target-Update
  int target_moved = state->target.alive;
  if (state->target.alive) {
    update_target(&state->target);
  }
  //Fadenkreuz Update. nur dann neu zeichnen
  if (state->crosshair.x != state->crosshair.old_x ||
      state->crosshair.y != state->crosshair.old_y || target_moved) {
    if (state->crosshair.x != state->crosshair.old_x ||
        state->crosshair.y != state->crosshair.old_y) {
      erase_cross(state->crosshair.old_x, state->crosshair.old_y,
                  &state->target);
    }

    draw_cross(state->crosshair.x, state->crosshair.y, COLOR_RED);
    //Das ist inkrementelles Rendering (sehr effizient für Embedded)
    state->crosshair.old_x = state->crosshair.x;
    state->crosshair.old_y = state->crosshair.y;
  }
}
//Input-Handling
void game_handle_input(GameState* state) {
  //Restart-Button
  int restart_pressed = restart_button_pressed();
  if (restart_pressed && !state->restart_prev) { //Flankenerkennung
    buzzer_stop();
    clear_screen();
    game_start(state);
  }
  state->restart_prev = restart_pressed;

  if (!state->running) return;

  int pressed = button_pressed();
  if (pressed && !state->button_prev) {
    play_shot_sound();
    state->sound_timer = 2;

    if (state->target.alive && check_hit(&state->crosshair, &state->target)) {
      state->score++;
      draw_square(state->target.x, state->target.y, state->target.half_size,
                  COLOR_WHITE);
      spawn_target(&state->target);
    }
  }
  state->button_prev = pressed;
}

int check_hit(const Crosshair* crosshair, const Target* target) {
  if (!target->alive) return 0;

  return (crosshair->x >= target->x - target->half_size &&
          crosshair->x <= target->x + target->half_size &&
          crosshair->y >= target->y - target->half_size &&
          crosshair->y <= target->y + target->half_size);
}
