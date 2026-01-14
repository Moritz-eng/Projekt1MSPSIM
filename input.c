#include "input.h"
#include "config.h"
#include <msp430.h>
#include <stdlib.h>

void input_init(void) {
    // Button init
    P3DIR &= ~BUTTON;
    P3REN |= BUTTON;
    P3OUT |= BUTTON;

    P4DIR &= ~RESTART_BUTTON;
    P4REN |= RESTART_BUTTON;
    P4OUT |= RESTART_BUTTON;

    // ADC init
    P6SEL |= BIT5 | BIT3;
    ADC12CTL0 = ADC12ON | ADC12SHT0_2;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL0 |= ADC12ENC;

    // Zufallszahl initialisieren
    srand(ADC12MEM0);
}

int button_pressed(void) {
    return !(P3IN & BUTTON);
}

int restart_button_pressed(void) {
    return !(P4IN & RESTART_BUTTON);
}

void joystick_update(Crosshair* crosshair, int max_speed) {
    // X-Achse lesen (A5)
    ADC12CTL0 &= ~ADC12ENC;
    ADC12MCTL0 = ADC12INCH_5;
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while (!(ADC12IFG & ADC12IFG0));
    int current_x = ADC12MEM0;
    ADC12IFG &= ~ADC12IFG0;

    // Y-Achse lesen (A3)
    ADC12CTL0 &= ~ADC12ENC;
    ADC12MCTL0 = ADC12INCH_3;
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while (!(ADC12IFG & ADC12IFG0));
    int current_y = ADC12MEM0;
    ADC12IFG &= ~ADC12IFG0;

    // Deadzone anwenden
    int dx = current_x - JOY_CENTER;
    int dy = current_y - JOY_CENTER;
    
    if (abs(dx) < DEADZONE) dx = 0;
    if (abs(dy) < DEADZONE) dy = 0;

    // Skalierung
    dx = (dx * max_speed) / 2047;
    dy = (dy * max_speed) / 2047;

    // Position aktualisieren
    crosshair->x += dx;
    crosshair->y -= dy;

    // Grenzen einhalten
    if (crosshair->x < 4) crosshair->x = 4;
    if (crosshair->x > DISPLAY_WIDTH - 4) crosshair->x = DISPLAY_WIDTH - 4;
    if (crosshair->y < UI_HEIGHT + 4) crosshair->y = UI_HEIGHT + 4;
    if (crosshair->y > DISPLAY_HEIGHT - 4) crosshair->y = DISPLAY_HEIGHT - 4;
}