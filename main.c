#include <msp430.h>
#include "ST7735.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define Display_x 128
#define Display_y 128
#define JOY_CENTER 2048
#define DEADZONE   5
#define MAX_SPEED 6
#define GAME_START_VALUE 300  // Startwert
#define UI_HEIGHT 25

#define BUTTON BIT7       // P3.7 Schuss-Button
#define RESTART_BUTTON BIT0  // P4.0 Neustart
#define BUZZER BIT5       // P2.5 Buzzer

// -------------------- Funktion Prototypen --------------------
void draw_cross(int x, int y, uint32_t color);
void draw_circle(int cx, int cy, int r, uint32_t color);
void erase_cross(int x, int y, int target_x, int target_y, int target_r, int target_alive);
void buzzer_start(void);
void buzzer_stop(void);
void spawn_target(int *x, int *y, int *vx, int *vy, int r);
void joystick_update(int *cross_x, int *cross_y, int max_speed);
void target_update_diff(int old_x, int old_y, int *x, int *y, int *vx, int *vy, int r);
int button_pressed(void);
void display_status(int score, int counter);
void game_start(int *cross_x, int *cross_y, int *target_x, int *target_y, int *target_vx, int *target_vy, int target_r);

// -------------------- Globale Variablen --------------------
int score = 0;
int game_counter = GAME_START_VALUE;
int game_running = 0; // 0 = nicht aktiv, 1 = aktiv
int last_score = -1;
int last_counter = -1;

// -------------------- Main --------------------
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    ST7735_interface_init();
    ST7735_display_init();
    draw(0, 0, Display_x, Display_y, 0xFFFFFFL);

    // Button init
    P3DIR &= ~BUTTON;
    P3REN |= BUTTON;
    P3OUT |= BUTTON;

    P4DIR &= ~RESTART_BUTTON;
    P4REN |= RESTART_BUTTON;
    P4OUT |= RESTART_BUTTON;

    // Buzzer init
    P2DIR |= BUZZER;
    P2OUT &= ~BUZZER;

    // ADC init
    P6SEL |= BIT5 | BIT3;
    ADC12CTL0 = ADC12ON | ADC12SHT0_2;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL0 |= ADC12ENC;

    srand(ADC12MEM0);

    int cross_x = Display_x/2, cross_y = Display_y/2;
    int o_cross_x = cross_x, o_cross_y = cross_y;

    int target_x, target_y, target_vx, target_vy;
    int target_r = 6;
    int target_alive = 1;

    draw_cross(cross_x, cross_y, 0x0000FFL);

    int button_prev = 1;
    int restart_prev = 1;
    int restart_pressed = 0;
    int old_target_x = 0, old_target_y = 0;

   

    while(1)
    {
        draw(0, UI_HEIGHT, Display_x, 1, 0x000000L); // Schwarze Linie
        // Neustart abfragen
        restart_pressed = !(P4IN & RESTART_BUTTON);
        if(restart_pressed && !restart_prev)
        {
            draw(0, 0, Display_x, Display_y, 0xFFFFFFL); // Display löschen
            game_start(&cross_x, &cross_y, &target_x, &target_y, &target_vx, &target_vy, target_r);
        }
        restart_prev = restart_pressed;

        if(game_running)
        {
            // Counter runterzählen
            if(game_counter > 0)
                game_counter--;
            else
                game_running = 0; // Spiel beendet

            // Joystick aktualisieren
            joystick_update(&cross_x, &cross_y, MAX_SPEED);

            // Fadenkreuz aktualisieren
            if(cross_x != o_cross_x || cross_y != o_cross_y)
            {
                erase_cross(o_cross_x, o_cross_y, target_x, target_y, target_r, target_alive);
                draw_cross(cross_x, cross_y, 0x0000FFL);
                o_cross_x = cross_x;
                o_cross_y = cross_y;
            }

            // Moorhuhn aktualisieren
            if(target_alive)
            {
                old_target_x = target_x;
                old_target_y = target_y;
                target_update_diff(old_target_x, old_target_y, &target_x, &target_y, &target_vx, &target_vy, target_r);
            }

            // Schuss abfragen
            int pressed = button_pressed();
            if(pressed && !button_prev)
            {
                buzzer_start();
                __delay_cycles(200000); 
                buzzer_stop();

                if(target_alive)
                {
                    int dx = cross_x - target_x;
                    int dy = cross_y - target_y;
                    if(dx*dx + dy*dy <= target_r*target_r)
                    {
                        score++;
                        draw_circle(target_x, target_y, target_r, 0xFFFFFFL);
                        spawn_target(&target_x, &target_y, &target_vx, &target_vy, target_r);
                    }
                }
            }
            button_prev = pressed;
        }

        // Statusanzeige (Score + Counter)
        display_status(score, game_counter);

        __delay_cycles(10000); // ca. 10ms pro Loop → verhindert Flackern
    }
}

// -------------------- Funktionen --------------------
void draw_cross(int x, int y, uint32_t color)
{
    draw(x-3, y,   7, 1, color);
    draw(x,   y-3, 1, 7, color);
}

void draw_circle(int cx, int cy, int r, uint32_t color)
{
    for(int y=-r; y<=r; y++)
        for(int x=-r; x<=r; x++)
            if(x*x + y*y <= r*r)
                draw(cx + x, cy + y, 1, 1, color);
}

void erase_cross(int x, int y, int target_x, int target_y, int target_r, int target_alive)
{
    int dx = x - target_x;
    int dy = y - target_y;
    if(target_alive && dx*dx + dy*dy <= target_r*target_r)
        draw_circle(target_x, target_y, target_r, 0xFF0000L);
    else
        draw_cross(x, y, 0xFFFFFFL);
}

void buzzer_start(void)
{
    P2DIR |= BUZZER;
    P2SEL |= BUZZER;
    TA0CCR0 = 499;
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 250;
    TA0CTL = TASSEL_2 + MC_1;
}

void buzzer_stop(void)
{
    TA0CTL = MC_0;
    P2SEL &= ~BUZZER;
    P2OUT &= ~BUZZER;
}

void spawn_target(int *x, int *y, int *vx, int *vy, int r)
{
    *x = r + 1 + rand() % (Display_x - 2*r - 2);
    *y = UI_HEIGHT + r + 1 + rand() % (Display_y - (UI_HEIGHT + 2*r + 2));
    *vx = (rand() % 3) + 1;
    *vy = (rand() % 3) + 1;
    if(rand()%2) *vx = -*vx;
    if(rand()%2) *vy = -*vy;
    draw_circle(*x, *y, r, 0xFF0000L);
}

void joystick_update(int *cross_x, int *cross_y, int max_speed)
{
    ADC12CTL0 &= ~ADC12ENC;
    ADC12MCTL0 = ADC12INCH_5;
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while(!(ADC12IFG & ADC12IFG0));
    int current_x = ADC12MEM0;
    ADC12IFG &= ~ADC12IFG0;

    ADC12CTL0 &= ~ADC12ENC;
    ADC12MCTL0 = ADC12INCH_3;
    ADC12CTL0 |= ADC12ENC | ADC12SC;
    while(!(ADC12IFG & ADC12IFG0));
    int current_y = ADC12MEM0;
    ADC12IFG &= ~ADC12IFG0;

    int dx = current_x - JOY_CENTER;
    int dy = current_y - JOY_CENTER;
    if(abs(dx) < DEADZONE) dx = 0;
    if(abs(dy) < DEADZONE) dy = 0;

    dx = (dx * max_speed) / 2047;
    dy = (dy * max_speed) / 2047;

    *cross_x += dx;
    *cross_y -= dy;

    if(*cross_x < 4) *cross_x = 4;
    if(*cross_x > Display_x-4) *cross_x = Display_x-4;
    if(*cross_y < UI_HEIGHT + 4) *cross_y = UI_HEIGHT + 4;
    if(*cross_y > Display_y-4) *cross_y = Display_y-4;
}

int button_pressed(void)
{
    return !(P3IN & BUTTON);
}

void display_status(int score, int counter)
{
    char buf[16];

    if(score != last_score)
    {
        sprintf(buf, "Score:%d   ", score); // Spaces zum Überschreiben
        setText(0,0,buf,0x000000L,0xFFFFFFL);
        last_score = score;
    }

    if(counter != last_counter)
    {
        sprintf(buf, "Val:%d   ", counter);
        setText(0,10,buf,0x000000L,0xFFFFFFL);
        last_counter = counter;
    }
}

void game_start(int *cross_x, int *cross_y, int *target_x, int *target_y, int *target_vx, int *target_vy, int target_r)
{
    score = 0;
    game_counter = GAME_START_VALUE;
    game_running = 1;
    last_score = -1;
    last_counter = -1;

    *cross_x = Display_x/2;
    *cross_y = Display_y/2;

    spawn_target(target_x, target_y, target_vx, target_vy, target_r);
    draw_cross(*cross_x, *cross_y, 0x0000FFL);
}

void target_update_diff(int old_x, int old_y, int *x, int *y, int *vx, int *vy, int r)
{
    // Bewegung
    *x += *vx;
    *y += *vy;

    // Wände abprallen
    if(*x - r < 0 || *x + r > Display_x) *vx = -*vx;
    if(*y - r < UI_HEIGHT + 2 || *y + r > Display_y) *vy = -*vy;

    // Bounding Box berechnen (nur Bereich, der sich ändern könnte)
    int min_x = old_x < *x ? old_x - r : *x - r;
    int max_x = old_x > *x ? old_x + r : *x + r;
    int min_y = old_y < *y ? old_y - r : *y - r;
    int max_y = old_y > *y ? old_y + r : *y + r;

    // Pixel-differenz zeichnen
    for(int py = min_y; py <= max_y; py++)
    {
        if(py < 0 || py >= Display_y) continue; // außerhalb Display
        for(int px = min_x; px <= max_x; px++)
        {
            if(px < 0 || px >= Display_x) continue; // außerhalb Display

            int old_in = (px - old_x)*(px - old_x) + (py - old_y)*(py - old_y) <= r*r;
            int new_in = (px - *x)*(px - *x) + (py - *y)*(py - *y) <= r*r;

            if(old_in && !new_in)
            {
                // Pixel gehört nicht mehr zum Kreis → Hintergrundfarbe
                draw(px, py, 1, 1, 0xFFFFFFL);
            }
            else if(!old_in && new_in)
            {
                // Pixel neu zum Kreis → Kreisfarbe
                draw(px, py, 1, 1, 0xFF0000L);
            }
        }
    }
}
