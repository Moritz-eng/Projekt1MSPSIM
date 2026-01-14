#include "game.h"
#include "config.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"

void game_init(GameState* state) {
    state->score = 0;
    state->counter = 0;
    state->running = 0;
    state->button_prev = 1;
    state->restart_prev = 1;
    
    state->crosshair.x = DISPLAY_WIDTH / 2;
    state->crosshair.y = DISPLAY_HEIGHT / 2;
    state->crosshair.old_x = state->crosshair.x;
    state->crosshair.old_y = state->crosshair.y;
    
    state->target.radius = TARGET_RADIUS;
    state->target.alive = 1;
}

void game_start(GameState* state) {
    state->score = 0;
    state->counter = GAME_START_VALUE;
    state->running = 1;
    
    state->crosshair.x = DISPLAY_WIDTH / 2;
    state->crosshair.y = DISPLAY_HEIGHT / 2;
    state->crosshair.old_x = state->crosshair.x;
    state->crosshair.old_y = state->crosshair.y;
    
    spawn_target(&state->target);
    draw_cross(state->crosshair.x, state->crosshair.y, COLOR_BLUE);
}

void game_update(GameState* state) {
    if (!state->running) return;

    // Counter runterzählen
    if (state->counter > 0) {
        state->counter--;
    } else {
        state->running = 0;
        return;
    }

    // Joystick aktualisieren
    joystick_update(&state->crosshair, MAX_SPEED);

    // Fadenkreuz aktualisieren
    if (state->crosshair.x != state->crosshair.old_x || 
        state->crosshair.y != state->crosshair.old_y) {
        
        erase_cross(state->crosshair.old_x, state->crosshair.old_y, &state->target);
        draw_cross(state->crosshair.x, state->crosshair.y, COLOR_BLUE);
        
        state->crosshair.old_x = state->crosshair.x;
        state->crosshair.old_y = state->crosshair.y;
    }

    // Target aktualisieren
    if (state->target.alive) {
        update_target(&state->target);
    }
}

void game_handle_input(GameState* state) {
    // Neustart-Button prüfen
    int restart_pressed = restart_button_pressed();
    if (restart_pressed && !state->restart_prev) {
        clear_screen();
        game_start(state);
    }
    state->restart_prev = restart_pressed;

    if (!state->running) return;

    // Schuss-Button prüfen
    int pressed = button_pressed();
    if (pressed && !state->button_prev) {
        play_shot_sound();

        if (state->target.alive && check_hit(&state->crosshair, &state->target)) {
            state->score++;
            draw_circle(state->target.x, state->target.y, state->target.radius, COLOR_WHITE);
            spawn_target(&state->target);
        }
    }
    state->button_prev = pressed;
}

int check_hit(const Crosshair* crosshair, const Target* target) {
    int dx = crosshair->x - target->x;
    int dy = crosshair->y - target->y;
    return (dx * dx + dy * dy <= target->radius * target->radius);
}