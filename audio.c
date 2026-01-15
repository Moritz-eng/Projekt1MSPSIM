#include "audio.h"
#include "config.h"
#include <msp430.h>

void audio_init(void) {
    // P2.5 als Ausgang setzen, aber initial Low (kein SEL)
    P2DIR |= BUZZER;
    P2OUT &= ~BUZZER;
}

void buzzer_start(void) {
    // 1. Pin P2.5 auf Timer-Funktion umschalten
    // P2.5 ist beim MSP430F5529 mit Timer TA2.2 verbunden
    P2DIR |= BUZZER;
    P2SEL |= BUZZER;  

    // 2. Timer 2 (TA2) konfigurieren
    TA2CTL = TACLR;             // Timer Reset
   
    // Frequenz einstellen (z.B. 1000 Ticks)
    TA2CCR0 = 1000;            
   
    // Duty Cycle für CCR2 (P2.5) einstellen
    TA2CCR2 = 500;              // 50% Duty Cycle
   
    // Output-Mode für CCR2 setzen (Reset/Set)
    TA2CCTL2 = OUTMOD_7;        
   
    // Timer starten: SMCLK, Up-Mode
    TA2CTL = TASSEL_2 | MC_1;  
}

void buzzer_stop(void) {
    // Timer stoppen
    TA2CTL = MC_0;
   
    // Pin wieder auf normale GPIO-Funktion zurückschalten
    P2SEL &= ~BUZZER;
   
    // Ausgang auf Low ziehen (damit er nicht dauerhaft piept, falls er bei High stoppte)
    P2OUT &= ~BUZZER;
}

void play_shot_sound(void) {
    // Kurzer Piep
    buzzer_start();
    // Etwas warten (Schussdauer)
    __delay_cycles(200000); // bei 1MHz ca 0.2s, bei schnellerem Takt entsprechend kürzer
    buzzer_stop();
}