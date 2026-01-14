#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

// Struktur für Zielobjekt
typedef struct {
    int x;
    int y;
    int vx;
    int vy;
    int radius;
    int alive;
} Target;

// Struktur für Fadenkreuz
typedef struct {
    int x;
    int y;
    int old_x;
    int old_y;
} Crosshair;

// Initialisierung
void graphics_init(void);

// Zeichenfunktionen
void draw_cross(int x, int y, uint32_t color);
void draw_circle(int cx, int cy, int r, uint32_t color);
void erase_cross(int x, int y, const Target* target);
void display_status(int score, int counter);

// Target-Verwaltung
void spawn_target(Target* target);
void update_target(Target* target);

// Utility
void clear_screen(void);
void draw_ui_separator(void);

#endif // GRAPHICS_H