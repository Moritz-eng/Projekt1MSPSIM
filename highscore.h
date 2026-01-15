#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <stdint.h>

#define MAX_HIGHSCORES 5
#define MAX_NAME_LEN 10

typedef struct {
    char name[MAX_NAME_LEN + 1]; // +1 für Null-Terminator
    int score;
} HighscoreEntry;

// Lädt, aktualisiert und speichert den Highscore, falls er hoch genug ist
void highscore_add(char* name, int score);

// Zeigt die Highscore-Liste auf dem Display an
void highscore_show(void);

// Löscht die Highscores (optional, zum Testen)
void highscore_clear(void);

#endif /* HIGHSCORE_H_ */