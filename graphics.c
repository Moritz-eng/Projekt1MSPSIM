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

/* ================================
   QUADRAT-ZEICHNEN
   ================================ */
void draw_square(int cx, int cy, int half, uint32_t color) {
    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {
            draw(cx + x, cy + y, 1, 1, color);
        }
    }
}

/* ================================
   FADENKREUZ LÖSCHEN
   ================================ */
void erase_cross(int x, int y, const Target* target) {
    // Horizontal
    for (int i = -3; i <= 3; i++) {
        int px = x + i;
        int py = y;

        if (px < 0 || px >= DISPLAY_WIDTH || py < 0 || py >= DISPLAY_HEIGHT)
            continue;

        int inside =
            target->alive &&
            px >= target->x - target->half_size &&
            px <= target->x + target->half_size &&
            py >= target->y - target->half_size &&
            py <= target->y + target->half_size;

        draw(px, py, 1, 1, inside ? COLOR_BLUE : COLOR_WHITE);
    }

    // Vertikal
    for (int i = -3; i <= 3; i++) {
        int px = x;
        int py = y + i;

        if (px < 0 || px >= DISPLAY_WIDTH || py < 0 || py >= DISPLAY_HEIGHT)
            continue;

        int inside =
            target->alive &&
            px >= target->x - target->half_size &&
            px <= target->x + target->half_size &&
            py >= target->y - target->half_size &&
            py <= target->y + target->half_size;

        draw(px, py, 1, 1, inside ? COLOR_BLUE : COLOR_WHITE);
    }
}

/* ================================
   TARGET SPAWNEN
   ================================ */
void spawn_target(Target* target) {
    target->x = target->half_size + 1 +
                rand() % (DISPLAY_WIDTH - 2 * target->half_size - 2);

    target->y = UI_HEIGHT + target->half_size + 1 +
                rand() % (DISPLAY_HEIGHT - (UI_HEIGHT + 2 * target->half_size + 2));

    target->vx = (rand() % 3) + 1;
    target->vy = (rand() % 3) + 1;

    if (rand() % 2) target->vx = -target->vx;
    if (rand() % 2) target->vy = -target->vy;

    target->alive = 1;

    draw_square(target->x, target->y, target->half_size, COLOR_BLUE);
}

/* ================================
   TARGET UPDATE (BEWEGUNG)
   ================================ */
void update_target(Target* target) {
    if (!target->alive) return;

    int old_x = target->x;
    int old_y = target->y;
    int size = (target->half_size * 2) + 1;

    // 1. Neue Position berechnen
    target->x += target->vx;
    target->y += target->vy;

    // 2. Wandkollision prüfen und Position korrigieren
    if (target->x - target->half_size < 0) {
        target->x = target->half_size;
        target->vx = -target->vx;
    }
    if (target->x + target->half_size >= DISPLAY_WIDTH) {
        target->x = DISPLAY_WIDTH - target->half_size - 1;
        target->vx = -target->vx;
    }
    if (target->y - target->half_size < UI_HEIGHT) {
        target->y = UI_HEIGHT + target->half_size;
        target->vy = -target->vy;
    }
    if (target->y + target->half_size >= DISPLAY_HEIGHT) {
        target->y = DISPLAY_HEIGHT - target->half_size - 1;
        target->vy = -target->vy;
    }

    // 3. Wenn keine Bewegung stattgefunden hat, abbrechen
    if (old_x == target->x && old_y == target->y) return;

    // 4. Inkrementelles Zeichnen (Streifen-Methode)
    // Wir löschen nur die Bereiche, die das Target verlassen hat,
    // und zeichnen nur die Bereiche neu, die neu hinzugekommen sind.
   
    // Horizontale Streifen (X-Achse)
    if (target->x > old_x) {
        // Bewegung nach rechts: links Streifen löschen, rechts Streifen zeichnen
        draw(old_x - target->half_size, old_y - target->half_size, target->x - old_x, size, COLOR_WHITE);
        draw(old_x + target->half_size + 1, target->y - target->half_size, target->x - old_x, size, COLOR_BLUE);
    } else if (target->x < old_x) {
        // Bewegung nach links: rechts Streifen löschen, links Streifen zeichnen
        draw(target->x + target->half_size + 1, old_y - target->half_size, old_x - target->x, size, COLOR_WHITE);
        draw(target->x - target->half_size, target->y - target->half_size, old_x - target->x, size, COLOR_BLUE);
    }

    // Vertikale Streifen (Y-Achse)
    if (target->y > old_y) {
        // Bewegung nach unten: oben Streifen löschen, unten Streifen zeichnen
        draw(target->x - target->half_size, old_y - target->half_size, size, target->y - old_y, COLOR_WHITE);
        draw(target->x - target->half_size, old_y + target->half_size + 1, size, target->y - old_y, COLOR_BLUE);
    } else if (target->y < old_y) {
        // Bewegung nach oben: unten Streifen löschen, oben Streifen zeichnen
        draw(target->x - target->half_size, target->y + target->half_size + 1, size, old_y - target->y, COLOR_WHITE);
        draw(target->x - target->half_size, target->y - target->half_size, size, old_y - target->y, COLOR_BLUE);
    }
}


/* ================================
   STATUSZEILE
   ================================ */
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
