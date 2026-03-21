#include <msp430.h>
#include <stdlib.h>
#include "config.h"
#include "input.h"

void input_init(void) {
  P3DIR &= ~BUTTON; 
  P3REN |= BUTTON;
  P3OUT |= BUTTON; //direction 0, ren 1 und out 1 = Eingang button mit pull-up

  P4DIR &= ~RESTART_BUTTON;
  P4REN |= RESTART_BUTTON;
  P4OUT |= RESTART_BUTTON; //direction 0, ren 1 und out 1 = Eingang button mit pull-up

  P6SEL |= BIT5 | BIT3; // P6.3 und P6.5 als ADC-Funktion (statt GPIO)
  ADC12CTL0 = ADC12ON | ADC12SHT0_2; // ADC einschalten + Sample-Hold-Time = 16 Takte
  ADC12CTL1 = ADC12SHP;  // Sample-Hold vom Sampler-Signal gesteuert
  ADC12CTL0 |= ADC12ENC; // ADC freigeben (Enable Conversion)

  srand(ADC12MEM0);
}

int button_pressed(void) { 
  return !(P3IN & BUTTON);
}  // Wenn Pin LOW (0), dann Button gedrückt

int restart_button_pressed(void) {
   return !(P4IN & RESTART_BUTTON); 
}  // Wenn Pin LOW (0), dann Button gedrückt

void joystick_update(Crosshair* crosshair, int max_speed) {
  // 1. X-Achse (P6.5) messen ADC muss immer zwischen x und y hin und her geschaltet werden
  ADC12CTL0 &= ~ADC12ENC; //ADC kurz deaktivieren zum Konfigurieren
  ADC12MCTL0 = ADC12INCH_5; //Kanal 5 (X-Achse) auswählen
  ADC12CTL0 |= ADC12ENC | ADC12SC; //ADC aktivieren + Wandlung starten
  while (!(ADC12IFG & ADC12IFG0)); //Warten bis fertig
  int current_x = ADC12MEM0; // Wert auslesen (0-4095)
  ADC12IFG &= ~ADC12IFG0; // Interrupt-Flag löschen
  // 2. Y-Achse (P6.3) messen
  ADC12CTL0 &= ~ADC12ENC;
  ADC12MCTL0 = ADC12INCH_3;
  ADC12CTL0 |= ADC12ENC | ADC12SC;
  while (!(ADC12IFG & ADC12IFG0));
  int current_y = ADC12MEM0;
  ADC12IFG &= ~ADC12IFG0;
  // 3. Abweichung von der Mitte berechnen
  int dx = current_x - JOY_CENTER;
  int dy = current_y - JOY_CENTER;
   // 4. Totzone ignorieren 
  if (abs(dx) < DEADZONE) dx = 0; //wenn in der totzone dann wird auf 0 gestzt.
  if (abs(dy) < DEADZONE) dy = 0;

  dx = (dx * max_speed) / 2047; //pixel die verschoben werden berechnen maximal 6
  dy = (dy * max_speed) / 2047; //oben ist 0 und unten ist 128

  crosshair->x += dx; //Neue Position
  crosshair->y -= dy;
  //Randbetrachtungen
  if (crosshair->x < 4) crosshair->x = 4;
  if (crosshair->x > DISPLAY_WIDTH - 4) crosshair->x = DISPLAY_WIDTH - 4; //Randbetrachtungen
  if (crosshair->y < UI_HEIGHT + 4) crosshair->y = UI_HEIGHT + 4;
  if (crosshair->y > DISPLAY_HEIGHT - 4) crosshair->y = DISPLAY_HEIGHT - 4;
}