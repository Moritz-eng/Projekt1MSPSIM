#include <msp430.h>
#include "config.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "keyboard.h"
#include <string.h>
#include "ST7735.h"

int main(void) {
    // Watchdog Timer stoppen
    WDTCTL = WDTPW | WDTHOLD;

    // Subsysteme initialisieren
    graphics_init();
    input_init();
    audio_init();

    // Tastatur-Phase
    KeyboardState keyboard;
    keyboard_init(&keyboard);
    
    clear_screen();
    keyboard_draw();
    draw_key_selected(0, 0);

    
    // Warte auf Namenseingabe
    while (!keyboard.done) {
        keyboard_handle_input(&keyboard);
        keyboard_update(&keyboard);
        __delay_cycles(10000);
    }
    
    // Name wurde eingegeben, Spiel starten
    clear_screen();
    
    // Optional: Name anzeigen
    if (keyboard.input_pos > 0) {
        char welcome[32];
        welcome[0] = '\0';
        strcat(welcome, "HI ");
        strcat(welcome, keyboard.input);
        setText(10, 10, welcome, COLOR_BLACK, COLOR_WHITE);
        __delay_cycles(2000000); // 2 Sekunden Pause
    }
    
    // Spiel initialisieren
    clear_screen();
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