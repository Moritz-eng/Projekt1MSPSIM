#include "audio.h"

#include <msp430.h>

#include "config.h"

void audio_init(void) {
  P2DIR |= BUZZER;
  P2OUT &= ~BUZZER;
}

void buzzer_start(void) {
  P2DIR |= BUZZER;
  P2SEL |= BUZZER;

  TA2CTL = TACLR;

  TA2CCR0 = 1000;

  TA2CCR2 = 500;

  TA2CCTL2 = OUTMOD_7;

  TA2CTL = TASSEL_2 | MC_1;
}

void buzzer_stop(void) {
  TA2CTL = MC_0;

  P2SEL &= ~BUZZER;

  P2OUT &= ~BUZZER;
}

void play_shot_sound(void) { buzzer_start(); }