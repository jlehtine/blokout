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
#include <string.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "game.h"
#include "overlay.h"
#include "highscore.h"
#include "highscoreui.h"

static Score *scoreTable;
static char name[32];
static unsigned int score;

/* Draw score table on top of existing graphics */
void drawScoreTableOverlay(Score *hs, int screenSize[2])
{
  static const char *txtHdr = "HIGH SCORE TABLE";
  static const void *fontHdr = GLUT_BITMAP_TIMES_ROMAN_24;
  static const void *fontEntry = GLUT_BITMAP_HELVETICA_12;
  int i, longest;
  char str[16];
  
  begin2DOverlay(screenSize[0], screenSize[1]);

  /* Draw background */
  glEnable(GL_BLEND);
  glColor4f(0.5, 0.5, 0.5, 0.5);
  drawRectangle(screenSize[0]/20, screenSize[0]*19/20,
                screenSize[1]/20, screenSize[1]*19/20);
  glDisable(GL_BLEND);

  /* Draw score table header */
  glColor3f(0, 1, 0);
  drawText(fontHdr,
           (screenSize[0] - getTextWidth(fontHdr, txtHdr))/2,
           screenSize[1]*9/10 - 24,
           txtHdr);

  /* Find the longest name in score table */
  longest = getTextWidth(fontEntry, "<ANONYMOUS>");
  for(i=0; i<HIGHSCORE_LENGTH; i++) {
    int j;

    j = getTextWidth(fontEntry, hs[i].name);
    if(j > longest)
      longest = j;
  }
  
  /* Draw score table entries */
  for(i=0; i<HIGHSCORE_LENGTH; i++) {
    char *s;
    int y;

    y = screenSize[1]*8/10 - (screenSize[1]*7/10)*i/HIGHSCORE_LENGTH;
    sprintf(str, "%d.", i+1);
    drawText(fontEntry,
             screenSize[0]/2 - longest/2 - 30 - getTextWidth(fontEntry, str),
             y, str);
    if(hs[i].name[0] == '\0')
      s = "<ANONYMOUS>";
    else
      s = hs[i].name;
    drawText(fontEntry, screenSize[0]/2 - getTextWidth(fontEntry, s)/2, y, s);
    sprintf(str, "%06u", hs[i].score);
    drawText(fontEntry,
             screenSize[0]/2 + longest/2 + 50 - getTextWidth(fontEntry, str),
             y, str);
  }
  
  end2DOverlay();
}

/* Begin new high score process */
void beginHighScore(Score *hs, unsigned int s)
{
  /* Initialize static variables */
  scoreTable = hs;
  score = s;
  name[0] = '\0';
}

/* Display high score screen */
void highScoreDisplay(int screenSize[2])
{
  static const char *txtCon = "CONGRATULATIONS";
  static const char *txtHS = "You just made it to the high score table!";
  static const char *txtEN = "Please enter your name:";
  static const void *fontCon = GLUT_BITMAP_TIMES_ROMAN_24;
  static const void *fontHS = GLUT_BITMAP_HELVETICA_18;
  static const void *fontN = GLUT_BITMAP_TIMES_ROMAN_24;

  begin2DOverlay(screenSize[0], screenSize[1]);

  /* Draw background */
  glEnable(GL_BLEND);
  glColor4f(0.5, 0.5, 0.5, 0.5);
  drawRectangle(screenSize[0]/20, screenSize[0]*19/20,
                screenSize[1]/20, screenSize[1]*19/20);
  glDisable(GL_BLEND);

  /* Draw texts */
  glColor3f(0, 1, 0);
  drawText(fontCon, (screenSize[0] - getTextWidth(fontCon, txtCon))/2,
           screenSize[1]*9/10 - 24, txtCon);
  drawText(fontHS, (screenSize[0] - getTextWidth(fontHS, txtHS))/2,
           screenSize[1]*7/10 - 18, txtHS);
  drawText(fontHS, (screenSize[0] - getTextWidth(fontHS, txtEN))/2,
           screenSize[1]*6/10 - 18, txtEN);

  /* Draw name written so far */
  glColor3f(0.8, 0.8, 0.1);
  drawText(fontN, (screenSize[0] - getTextWidth(fontN, name))/2,
           screenSize[1]/2 - 12, name);
  
  end2DOverlay();
}

/* Read keyboard */
int highScoreKeyboard(unsigned char key)
{
  /* Check if key is one of the "legal" ones */
  if(key >= 32 && key < 256 && key != 127) {
    int i;
    
    /* Is there space for letter? */
    i = strlen(name);
    if(i < 31) {
      name[i] = key;
      name[i+1] = '\0';
    }
    glutPostRedisplay();
    return 0;
  }

  /* Check if it's a return */
  if(key == 13) {
    /* Add name and score to score table */
    addHighScore(scoreTable, score, name);

    /* Save new high scores */
    if(saveScoreTable(scoreTable, "scores.dat", "scores.bak") != 0)
      fprintf(stderr, "WARNING! Could not save high score table...\n");
    return 1;
  }

  /* Check if it's a delete */
  if(key == 8) {
    int i;
    
    /* Delete one character if possible */
    i = strlen(name);
    if(i > 0)
      name[i-1] = '\0';
    return 0;
  }
  
  return 0;
}
