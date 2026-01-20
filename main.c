#include <msp430.h>
#include "config.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "keyboard.h"
#include <string.h>
#include "ST7735.h"
#include "highscore.h"

int main(void) {
    // Watchdog Timer stoppen
    WDTCTL = WDTPW | WDTHOLD;

    // Subsysteme initialisieren
    graphics_init();
    input_init();
    audio_init();
   

    while (1) { // Äußere Schleife für kompletten Spiel-Neustart (inkl. Name)
       
        // 1. Namenseingabe
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
       
        // Name fertig
        clear_screen();
       
        // 2. Spiel Starten
        GameState game;
        game_init(&game);
        game_start(&game); // Startet Timer und setzt running=1

        // Hauptspiel-Loop
        while (game.running) {
            draw_ui_separator();
            game_handle_input(&game);
            game_update(&game);
            display_status(game.score, game.counter);
            __delay_cycles(10000);
        }

        // 3. Spiel Vorbei - Highscore Logik
        play_shot_sound(); // Kleiner Soundeffekt am Ende
       
        // Highscore speichern (falls gut genug)
        highscore_add(keyboard.input, game.score);

        // Highscores anzeigen
        highscore_show();

        // 4. Warten auf Neustart-Button (Restart)
        // Wir warten hier, bis der Nutzer drückt, um zur Namenseingabe zurückzukehren
        while (!restart_button_pressed()) {
             // Warten...
        }
        // Entprellen
        while (restart_button_pressed());
        __delay_cycles(100000);
    }
}