#include "highscore.h"
#include <msp430.h>
#include <string.h>
#include "ST7735.h"
#include "config.h"
#include <stdio.h>

// Adresse von Info Memory Segment D beim MSP430F5529
#define SEGMENT_D_START 0x1800

// Zeiger auf den Flash-Bereich
static HighscoreEntry* flash_scores = (HighscoreEntry*) SEGMENT_D_START;

// Interne Funktion: Schreibt das gesamte Array in den Flash
void write_to_flash(HighscoreEntry entries[MAX_HIGHSCORES]) {
    unsigned int *flash_ptr = (unsigned int *)SEGMENT_D_START;
    unsigned int *ram_ptr = (unsigned int *)entries;
    int i;
    int word_count = sizeof(HighscoreEntry) * MAX_HIGHSCORES / 2;

    // 1. Flash Controller konfigurieren
    FCTL3 = FWKEY;              // Lock-Bit löschen
    FCTL1 = FWKEY + ERASE;      // Erase-Bit setzen

    // 2. Dummy-Write zum Starten des Löschvorgangs (Segment löschen)
    *flash_ptr = 0;

    // 3. Warten bis Löschen fertig
    while(FCTL3 & BUSY);

    // 4. Schreibmodus aktivieren
    FCTL1 = FWKEY + WRT;

    // 5. Daten wortweise kopieren
    for (i = 0; i < word_count; i++) {
        *flash_ptr++ = *ram_ptr++;
        while(FCTL3 & BUSY); // Warten bis Wort geschrieben
    }

    // 6. Schreibmodus beenden und Flash sperren
    FCTL1 = FWKEY;
    FCTL3 = FWKEY + LOCK;
}

void highscore_add(char* name, int score) {
    HighscoreEntry current_scores[MAX_HIGHSCORES];
    int i, j;

    // 1. Aktuelle Scores aus Flash in RAM kopieren
    for(i = 0; i < MAX_HIGHSCORES; i++) {
        // Prüfen, ob Flash leer ist (0xFFFF) -> dann als leerer Eintrag behandeln
        if (flash_scores[i].score == -1 || flash_scores[i].score == 0xFFFF) {
            current_scores[i].score = 0;
            strcpy(current_scores[i].name, "---");
        } else {
            current_scores[i] = flash_scores[i];
        }
    }

    // 2. Prüfen ob neuer Score in die Liste gehört
    if (score <= current_scores[MAX_HIGHSCORES-1].score) {
        return; // Score zu niedrig für die Top-Liste
    }

    // 3. Einfügen und Sortieren (Insertion Logic)
    for (i = 0; i < MAX_HIGHSCORES; i++) {
        if (score > current_scores[i].score) {
            // Nachfolgende Scores nach unten schieben
            for (j = MAX_HIGHSCORES - 1; j > i; j--) {
                current_scores[j] = current_scores[j-1];
            }
            // Neuen Score einfügen
            current_scores[i].score = score;
            strncpy(current_scores[i].name, name, MAX_NAME_LEN);
            current_scores[i].name[MAX_NAME_LEN] = '\0'; // Sicherstellen null-terminiert
            break;
        }
    }

    // 4. Neue Liste zurück in den Flash schreiben
    write_to_flash(current_scores);
}

void highscore_show(void) {
    char buf[32];
    int i;

    // Bildschirm löschen für Highscore-Anzeige
    draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BLACK);

    setText(20, 10, "HIGHSCORES", COLOR_RED, COLOR_BLACK);
    draw(20, 22, 88, 1, COLOR_WHITE); // Unterstreichung

    for (i = 0; i < MAX_HIGHSCORES; i++) {
        int y_pos = 40 + (i * 15);
        int current_score = flash_scores[i].score;

        // Wenn Eintrag leer/ungültig (Flash erased state ist 0xFFFF -> -1 signed int)
        if (current_score == -1 || current_score == 0xFFFF) {
            sprintf(buf, "%d. ---    0", i + 1);
        } else {
            // Formatierung: "1. NAME   100"
            sprintf(buf, "%d. %-6s %3d", i + 1, flash_scores[i].name, current_score);
        }
       
        // Farben: Platz 1 Gold (Gelb), Rest Weiß
        unsigned long color = (i == 0) ? 0xFFFF00 : COLOR_WHITE;
        setText(10, y_pos, buf, color, COLOR_BLACK);
    }
   
    setText(10, 110, "PRESS RESTART", COLOR_BLUE, COLOR_BLACK);
}

void highscore_clear(void) {
    // Leeres Array erzeugen und schreiben
    HighscoreEntry empty[MAX_HIGHSCORES];
    for(int i=0; i<MAX_HIGHSCORES; i++) {
        empty[i].score = 0;
        strcpy(empty[i].name, "---");
    }
    write_to_flash(empty);
}