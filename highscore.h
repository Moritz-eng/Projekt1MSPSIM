#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <stdint.h>

#define MAX_HIGHSCORES 5
#define MAX_NAME_LEN 10

typedef struct {
  char name[MAX_NAME_LEN + 1];
  int score;
} HighscoreEntry;

void highscore_add(char* name, int score);

void highscore_show(void);

void highscore_clear(void);

#endif