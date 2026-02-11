#include "highscore.h"

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "ST7735.h"
#include "config.h"

#define SEGMENT_D_START 0x1800

static HighscoreEntry *flash_scores = (HighscoreEntry *)SEGMENT_D_START;

void write_to_flash(HighscoreEntry entries[MAX_HIGHSCORES]) {
  unsigned int *flash_ptr = (unsigned int *)SEGMENT_D_START;
  unsigned int *ram_ptr = (unsigned int *)entries;
  int i;
  int word_count = sizeof(HighscoreEntry) * MAX_HIGHSCORES / 2;

  FCTL3 = FWKEY;
  FCTL1 = FWKEY + ERASE;

  *flash_ptr = 0;

  while (FCTL3 & BUSY);

  FCTL1 = FWKEY + WRT;

  for (i = 0; i < word_count; i++) {
    *flash_ptr++ = *ram_ptr++;
    while (FCTL3 & BUSY);
  }

  FCTL1 = FWKEY;
  FCTL3 = FWKEY + LOCK;
}

void highscore_add(char *name, int score) {
  HighscoreEntry current_scores[MAX_HIGHSCORES];
  int i, j;

  for (i = 0; i < MAX_HIGHSCORES; i++) {
    if (flash_scores[i].score == -1 || flash_scores[i].score == 0xFFFF) {
      current_scores[i].score = 0;
      strcpy(current_scores[i].name, "--------");
    } else {
      current_scores[i] = flash_scores[i];
    }
  }

  if (score <= current_scores[MAX_HIGHSCORES - 1].score) {
    return;
  }

  for (i = 0; i < MAX_HIGHSCORES; i++) {
    if (score > current_scores[i].score) {
      for (j = MAX_HIGHSCORES - 1; j > i; j--) {
        current_scores[j] = current_scores[j - 1];
      }

      current_scores[i].score = score;
      strncpy(current_scores[i].name, name, MAX_NAME_LEN);
      current_scores[i].name[MAX_NAME_LEN] = '\0';
      break;
    }
  }

  write_to_flash(current_scores);
}

void highscore_show(void) {
  char buf[32];
  int i;

  draw(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BLACK);

  setText(20, 10, "HIGHSCORES", COLOR_RED, COLOR_BLACK);
  draw(20, 22, 88, 1, COLOR_WHITE);

  for (i = 0; i < MAX_HIGHSCORES; i++) {
    int y_pos = 35 + (i * 15);
    int current_score = flash_scores[i].score;

    if (current_score == -1 || current_score == 0xFFFF) {
      sprintf(buf, "%d. --------  0", i + 1);
    } else {
      sprintf(buf, "%d. %-8s %3d", i + 1, flash_scores[i].name, current_score);
    }

    unsigned long color = (i == 0) ? 0xFFFF00 : COLOR_WHITE;
    setText(10, y_pos, buf, color, COLOR_BLACK);
  }

  setText(10, 110, "Press Restart", COLOR_BLUE, COLOR_BLACK);
}