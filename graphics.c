#include "graphics.h"
#include "config.h"
#include "ST7735.h"
#include <stdlib.h>
#include <stdio.h>

static int last_score = -1;
static int last_counter = -1;

void graphics_init(void) {
    ST7735_interface_init();
    ST7735_display_init();
    clear_screen();
}

void clear_screen(void) {
    draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_WHITE);
    last_score = -1;
    last_counter = -1;
}

void draw_ui_separator(void) {
    draw(0, UI_HEIGHT, DISPLAY_WIDTH, 1, COLOR_BLACK);
}

void draw_cross(int x, int y, uint32_t color) {
    draw(x - 3, y, 7, 1, color);
    draw(x, y - 3, 1, 7, color);
}

void draw_circle(int cx, int cy, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                draw(cx + x, cy + y, 1, 1, color);
            }
        }
    }
}

void erase_cross(int x, int y, const Target* target) {
    // Horizontale Linie löschen
    for (int i = -3; i <= 3; i++) {
        int px = x + i;
        int py = y;

        if (px < 0 || px >= DISPLAY_WIDTH || py < 0 || py >= DISPLAY_HEIGHT) 
            continue;

        int dx = px - target->x;
        int dy = py - target->y;

        if (target->alive && dx * dx + dy * dy <= target->radius * target->radius) {
            draw(px, py, 1, 1, COLOR_RED);
        } else {
            draw(px, py, 1, 1, COLOR_WHITE);
        }
    }

    // Vertikale Linie löschen
    for (int i = -3; i <= 3; i++) {
        int px = x;
        int py = y + i;

        if (px < 0 || px >= DISPLAY_WIDTH || py < 0 || py >= DISPLAY_HEIGHT) 
            continue;

        int dx = px - target->x;
        int dy = py - target->y;

        if (target->alive && dx * dx + dy * dy <= target->radius * target->radius) {
            draw(px, py, 1, 1, COLOR_RED);
        } else {
            draw(px, py, 1, 1, COLOR_WHITE);
        }
    }
}

void spawn_target(Target* target) {
    target->x = target->radius + 1 + rand() % (DISPLAY_WIDTH - 2 * target->radius - 2);
    target->y = UI_HEIGHT + target->radius + 1 + 
                rand() % (DISPLAY_HEIGHT - (UI_HEIGHT + 2 * target->radius + 2));
    target->vx = (rand() % 3) + 1;
    target->vy = (rand() % 3) + 1;
    if (rand() % 2) target->vx = -target->vx;
    if (rand() % 2) target->vy = -target->vy;
    target->alive = 1;
    
    draw_circle(target->x, target->y, target->radius, COLOR_RED);
}

void update_target(Target* target) {
    if (!target->alive) return;

    int old_x = target->x;
    int old_y = target->y;

    // Bewegung
    target->x += target->vx;
    target->y += target->vy;

    // Wände abprallen
    if (target->x - target->radius < 0 || target->x + target->radius > DISPLAY_WIDTH) {
        target->vx = -target->vx;
    }
    if (target->y - target->radius < UI_HEIGHT + 2 || target->y + target->radius > DISPLAY_HEIGHT) {
        target->vy = -target->vy;
    }

    // Bounding Box berechnen
    int min_x = (old_x < target->x ? old_x : target->x) - target->radius;
    int max_x = (old_x > target->x ? old_x : target->x) + target->radius;
    int min_y = (old_y < target->y ? old_y : target->y) - target->radius;
    int max_y = (old_y > target->y ? old_y : target->y) + target->radius;

    // Pixel-differenz zeichnen
    for (int py = min_y; py <= max_y; py++) {
        if (py < 0 || py >= DISPLAY_HEIGHT) continue;
        
        for (int px = min_x; px <= max_x; px++) {
            if (px < 0 || px >= DISPLAY_WIDTH) continue;

            int old_in = (px - old_x) * (px - old_x) + (py - old_y) * (py - old_y) 
                         <= target->radius * target->radius;
            int new_in = (px - target->x) * (px - target->x) + (py - target->y) * (py - target->y) 
                         <= target->radius * target->radius;

            if (old_in && !new_in) {
                draw(px, py, 1, 1, COLOR_WHITE);
            } else if (!old_in && new_in) {
                draw(px, py, 1, 1, COLOR_RED);
            }
        }
    }
}

void display_status(int score, int counter) {
    char buf[16];

    if (score != last_score) {
        sprintf(buf, "Score:%d   ", score);
        setText(0, 0, buf, COLOR_BLACK, COLOR_WHITE);
        last_score = score;
    }

    if (counter != last_counter) {
        sprintf(buf, "Time:%d   ", counter);
        setText(0, 10, buf, COLOR_BLACK, COLOR_WHITE);
        last_counter = counter;
    }
}