#include <msp430.h>
#include "config.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"

int main(void) {
    // Watchdog Timer stoppen
    WDTCTL = WDTPW | WDTHOLD;

    // Subsysteme initialisieren
    graphics_init();
    input_init();
    audio_init();

    // Spiel initialisieren
    GameState game;
    game_init(&game);

    // Initiales Fadenkreuz zeichnen
    draw_cross(game.crosshair.x, game.crosshair.y, COLOR_BLUE);

    // Hauptspiel-Loop
    while (1) {
        // UI-Trennlinie zeichnen
        draw_ui_separator();

        // Eingaben verarbeiten
        game_handle_input(&game);

        // Spiel aktualisieren
        game_update(&game);

        // Status anzeigen
        display_status(game.score, game.counter);

        // Kleine Verzögerung (ca. 10ms)
        __delay_cycles(10000);
    }
}