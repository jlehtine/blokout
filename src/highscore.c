/*************************************************************************
 * Copyright (C) 1998-1999 Johannes Lehtinen
 * Copyright (C) 1998-1999 Petri Salmi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "highscore.h"

static unsigned int getChecksum(void *ptr, size_t len);

/* Calculate checksum for memory block */
static unsigned int getChecksum(void *ptr, size_t len)
{
  int i, j;
  unsigned int checksum;
  unsigned char *cp;

  checksum = 0;
  cp = ptr;
  j = 0;
  for(i=0; i<len/sizeof(unsigned char); i++) {
    checksum ^= ((unsigned int)(cp[i])) << j;
    j += sizeof(unsigned char);
    if(j + sizeof(unsigned char) > sizeof(unsigned int))
      j = 0;
  }
  return checksum;
}

/* Allocate new high score table */
Score *newScoreTable(void)
{
  Score *hs;

  /* Allocate space for high scores */
  if((hs = malloc(sizeof(Score) * HIGHSCORE_LENGTH)) == NULL) {
    fprintf(stderr, "Error, not enough memory\n");
    exit(1);
  }

  /* Initialize high scores */
  memset(hs, 0, sizeof(Score) * HIGHSCORE_LENGTH);
  return hs;
}

/* Delete high score table */
void deleteScoreTable(Score *hs)
{
  free(hs);
}

/* Load high score table. Returns 0 on success. */
int loadScoreTable(Score *hs, char *fname)
{
  FILE *f;
  unsigned int checksum;

  /* Check file for modifications and read scores */
  if((f = fopen(fname, "rb")) == NULL ||
     fread(&checksum, sizeof(unsigned int), 1, f) != 1 ||
     fread(hs, sizeof(Score), HIGHSCORE_LENGTH, f) != HIGHSCORE_LENGTH ||
     getChecksum(hs, sizeof(Score) * HIGHSCORE_LENGTH) != checksum) {
    if(f != NULL)
      fclose(f);
    memset(hs, 0, sizeof(Score) * HIGHSCORE_LENGTH);
    return -1;
  }
  fclose(f);
  return 0;
}

/* Saves high score table. Returns 0 on success. */
int saveScoreTable(Score *hs, char *fname, char *backupname)
{
  FILE *f;
  char *tmpname;
  unsigned int checksum;

  /* Calculate checksum for high score table */
  checksum = getChecksum(hs, sizeof(Score) * HIGHSCORE_LENGTH);

  /* Create temporary file name */
  if((tmpname = malloc(sizeof(char) * (strlen(fname) + 20))) == NULL) {
    fprintf(stderr, "Error, not enough memory\n");
    exit(1);
  }
  sprintf(tmpname, "%s.%u", fname, (unsigned int)getpid());

  /* Open temporary file and write data */
  if((f = fopen(tmpname, "wb")) == NULL ||
     fwrite(&checksum, sizeof(unsigned int), 1, f) != 1 ||
     fwrite(hs, sizeof(Score), HIGHSCORE_LENGTH, f) != HIGHSCORE_LENGTH) {
    if(f != NULL) {
      fclose(f);
      remove(tmpname);
    }
    free(tmpname);
    return -1;
  }

  /* Close file and check for success */
  if(fclose(f) != 0) {
    remove(tmpname);
    free(tmpname);
    return -1;
  }

  /* Replace old score file */
  remove(backupname);
  rename(fname, backupname);
  if(rename(tmpname, fname) != 0) {
    free(tmpname);
    return -1;
  }
  free(tmpname);
  return 0;
}

/* Returns nonzero if score makes it to the high score table */
int isHighScore(Score *hs, int score)
{
  return (score > hs[HIGHSCORE_LENGTH-1].score ? 1 : 0);
}

/* Add new score to high score table */
void addHighScore(Score *hs, int score, char *name)
{
  int pos;
  
  /* First check that the score really makes it and that name is legal */
  if(score <= hs[HIGHSCORE_LENGTH-1].score ||
     strlen(name) > 31) {
    fprintf(stderr,
            "Error, internal consistency check in highscore:addHighScore()\n");
    exit(1);
  }

  /* Find the correct place for score */
  for(pos = 0; pos < HIGHSCORE_LENGTH; pos++)
    if(score > hs[pos].score)
      break;

  /* Move other scores backwards if necessary */
  if(pos < HIGHSCORE_LENGTH-1)
    memmove(hs+pos+1, hs+pos, sizeof(Score) * (HIGHSCORE_LENGTH-pos-1));

  /* Write the new score to table */
  hs[pos].score = score;
  strcpy(hs[pos].name, name);
}
