#include "audio.h"

#include <msp430.h>
#include "config.h"

/**
 * Initialisiert die Audio-Hardware
 * Setzt den Buzzer-Pin als Ausgang und stellt sicher, dass er initial aus ist
 */
void audio_init(void) {
  P2DIR |= BUZZER;        
  // Setzt das entsprechende Bit im Port-2-Richtungsregister als Ausgang (1 = Ausgang)
  P2OUT &= ~BUZZER;       
  // Setzt den Ausgangspegel auf Low (0), damit der Buzzer zu Beginn aus ist
}

/**
 * Startet den Buzzer mit PWM-Signal zur Tonerzeugung
 * Konfiguriert Timer A2 im PWM-Modus mit einer bestimmten Frequenz
 */
void buzzer_start(void) {
  P2DIR |= BUZZER;        
  // Stellt sicher, dass der Pin als Ausgang konfiguriert ist (kann entfallen, wenn audio_init() zuvor aufgerufen wurde)
  P2SEL |= BUZZER;        
  // Aktiviert die Peripherie-Funktion für diesen Pin: Der Pin wird mit dem Timer-Ausgang verbunden (PWM-Modus)

  TA2CTL = TACLR;         
  // Timer A2 zurücksetzen: Alle Timer-Register werden gelöscht, Zähler steht auf 0

  TA2CCR0 = 1000;         
  // Capture/Compare-Register 0: Setzt die Periodendauer des PWM-Signals
    // Bei Taktquelle SMCLK (1 MHz) ergibt sich: Frequenz = 1.000.000 / 1000 = 1000 Hz
  TA2CCR2 = 500;          
  // Capture/Compare-Register 2: Setzt die Einschaltdauer (Duty Cycle)
   // Bei CCR0 = 1000 und CCR2 = 500 beträgt das Tastverhältnis 50% (500 Takte High, 500 Takte Low)

  TA2CCTL2 = OUTMOD_7;    
  // Setzt den Ausgabemodus für Compare-Channel 2 auf Modus 7 (Reset/Set)
    // Bei diesem PWM-Modus: Ausgang wird bei Zählerstand 0 gesetzt (High) und bei CCR2 zurückgesetzt (Low)

  TA2CTL = TASSEL_2 | MC_1; 
  // Startet den Timer A2:
    // TASSEL_2: Wählt SMCLK (Sub-Main Clock, typisch 1 MHz) als Taktquelle
    // MC_1: Setzt den Timer in den Aufwärts-Modus (zählt von 0 bis CCR0, dann zurück auf 0)
}

/**
 * Stoppt den Buzzer sofort
 * Deaktiviert den Timer und trennt den Pin vom Timer-Ausgang
 */
void buzzer_stop(void) {
  TA2CTL = MC_0;          
  // Stoppt den Timer: Setzt den Modus-Control auf 0 (Timer läuft nicht mehr)

  P2SEL &= ~BUZZER;       
  // Deaktiviert die Peripherie-Funktion des Pins: Trennt den Pin vom Timer-Ausgang

  P2OUT &= ~BUZZER;       
// Setzt den Pin-Pegel explizit auf Low, um sicherzustellen, dass der Buzzer aus ist

}


/**
 * Spielt einen Schuss-Ton ab
 * Aktuell wird einfach der Buzzer gestartet (ohne spezifische Anpassung)
 * Hier könnte später eine spezielle Abfolge oder Frequenz für den Schuss-Ton implementiert werden
 */
void play_shot_sound(void) { buzzer_start(); }      
   // Startet den Buzzer mit der konfigurierten PWM-Frequenz (aktuell 1000 Hz)
