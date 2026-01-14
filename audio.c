#include "audio.h"
#include "config.h"
#include <msp430.h>

void audio_init(void) {
    P2DIR |= BUZZER;
    P2OUT &= ~BUZZER;
}

void buzzer_start(void) {
    P2DIR |= BUZZER;
    P2SEL |= BUZZER;  // Timer-Funktion

    TA0CTL = TACLR;            // Reset Timer
    TA0CCR0 = 1000;            // Period
    TA0CCR1 = 500;             // 50% duty
    TA0CCTL1 = OUTMOD_7;       // Reset/Set
    TA0CTL = TASSEL_2 | MC_1;  // SMCLK, Up-Mode
}

void buzzer_stop(void) {
    TA0CTL = MC_0;
    P2SEL &= ~BUZZER;
    P2OUT &= ~BUZZER;
}

void play_shot_sound(void) {
    buzzer_start();
    __delay_cycles(200000);
    buzzer_stop();
}