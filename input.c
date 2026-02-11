#include "input.h"

#include <msp430.h>
#include <stdlib.h>

#include "config.h"

void input_init(void) {
  P3DIR &= ~BUTTON;
  P3REN |= BUTTON;
  P3OUT |= BUTTON;

  P4DIR &= ~RESTART_BUTTON;
  P4REN |= RESTART_BUTTON;
  P4OUT |= RESTART_BUTTON;

  P6SEL |= BIT5 | BIT3;
  ADC12CTL0 = ADC12ON | ADC12SHT0_2;
  ADC12CTL1 = ADC12SHP;
  ADC12CTL0 |= ADC12ENC;

  srand(ADC12MEM0);
}

int button_pressed(void) { return !(P3IN & BUTTON); }

int restart_button_pressed(void) { return !(P4IN & RESTART_BUTTON); }

void joystick_update(Crosshair* crosshair, int max_speed) {
  ADC12CTL0 &= ~ADC12ENC;
  ADC12MCTL0 = ADC12INCH_5;
  ADC12CTL0 |= ADC12ENC | ADC12SC;
  while (!(ADC12IFG & ADC12IFG0));
  int current_x = ADC12MEM0;
  ADC12IFG &= ~ADC12IFG0;

  ADC12CTL0 &= ~ADC12ENC;
  ADC12MCTL0 = ADC12INCH_3;
  ADC12CTL0 |= ADC12ENC | ADC12SC;
  while (!(ADC12IFG & ADC12IFG0));
  int current_y = ADC12MEM0;
  ADC12IFG &= ~ADC12IFG0;

  int dx = current_x - JOY_CENTER;
  int dy = current_y - JOY_CENTER;

  if (abs(dx) < DEADZONE) dx = 0;
  if (abs(dy) < DEADZONE) dy = 0;

  dx = (dx * max_speed) / 2047;
  dy = (dy * max_speed) / 2047;

  crosshair->x += dx;
  crosshair->y -= dy;

  if (crosshair->x < 4) crosshair->x = 4;
  if (crosshair->x > DISPLAY_WIDTH - 4) crosshair->x = DISPLAY_WIDTH - 4;
  if (crosshair->y < UI_HEIGHT + 4) crosshair->y = UI_HEIGHT + 4;
  if (crosshair->y > DISPLAY_HEIGHT - 4) crosshair->y = DISPLAY_HEIGHT - 4;
}