#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// Display Konfiguration
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128
#define UI_HEIGHT 25

// Joystick Konfiguration
#define JOY_CENTER 2048
#define DEADZONE 5
#define MAX_SPEED 6

// Spiel Konfiguration
#define GAME_START_VALUE 300
#define TARGET_RADIUS 6

// Pin Definitionen
#define BUTTON BIT7          // P3.7 Schuss-Button
#define RESTART_BUTTON BIT0  // P4.0 Neustart
#define BUZZER BIT5          // P2.5 Buzzer

// Farben (RGB 24-bit)
#define COLOR_WHITE 0xFFFFFFL
#define COLOR_BLACK 0x000000L
#define COLOR_BLUE 0x0000FFL
#define COLOR_RED 0xFF0000L

#endif // CONFIG_H