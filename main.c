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

void show_instruction_screen(void) {
    clear_screen();
   
    setText(15, 10, "INSTRUCTIONS", COLOR_RED, COLOR_WHITE);
    draw(15, 22, 70, 1, COLOR_BLACK); // Unterstreichung

    setText(5, 40, "- MOVE", COLOR_BLACK, COLOR_WHITE);
    setText(15, 52, "Joystick", COLOR_BLUE, COLOR_WHITE);

    setText(5, 70, "- SHOOT/OK:", COLOR_BLACK, COLOR_WHITE);
    setText(15, 82, "R. LOWER BUTTON", COLOR_BLUE, COLOR_WHITE);

    setText(5, 100, "- Reset:", COLOR_BLACK, COLOR_WHITE);
    setText(15, 112, "R. UP BUTTON", COLOR_BLUE, COLOR_WHITE);

    // Warten bis der Restart-Button (R. oben) gedrückt wird
    while (!button_pressed());
    while (button_pressed()); // Entprellen
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
    // Watchdog Timer stoppen
    WDTCTL = WDTPW | WDTHOLD;

    // Subsysteme initialisieren
    graphics_init();
    input_init();
    audio_init();

    UCSCTL4 |= SELA_2;
    TA0CCR0 = 655;
    TA0CTL = TASSEL_1 | MC_1 | TACLR;

    // Anleitung vor der Namenseingabe anzeigen
    show_instruction_screen();


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

        if (keyboard.input[0] != '\0'){
            strncpy(last_used_name, keyboard.input, 15);
        }
       
        show_start_screen(keyboard.input); 

        while (!button_pressed()) {
        }

        while (button_pressed());
        __delay_cycles(100000);

        // Name fertig
        clear_screen();
       
        // 2. Spiel Starten
        GameState game;
        game_init(&game);
        game_start(&game); // Startet Timer und setzt running=1

        // Hauptspiel-Loop
        while (game.running) {
            while (!(TA0CTL & TAIFG));
            TA0CTL &= ~TAIFG;

            draw_ui_separator();
            game_handle_input(&game);
            game_update(&game);
            display_status(game.score, game.counter);
        }
        buzzer_stop();

        // 3. Spiel Vorbei - Highscore Logik
        
       
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