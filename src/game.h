/* $Id$ */

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

#ifndef __game_h_included__
#define __game_h_included__

#include "forms.h"
#include "blocks.h"
#include "tube.h"
#include "highscore.h"

typedef struct Game Game;

struct Game {
  int numFormsInGame;  /* number of forms that are in game */
  Form **formsInGame;  /* forms that are in game */
  Score *scoreTable;   /* high score table */
  int level;           /* current level (speed) 1-10 */
  int startLevel;      /* the level to start */
  int numCurrentBlock; /* number of current block */
  int cubesDropped;    /* number of individual cubes dropped so far */
  int score;           /* score */
  int levelsDescended; /* how many levels are descended */
  float levelFactor;   /* factor for scoring */
  int solidBlocks;     /* should we use solid or wireframe modeling */
  int backgroundColor; /* should we draw background too */
  int height;          /* height of the tube */
  int xWidth;          /* x-width of the tube */
  int yWidth;          /* y-width of the tube */
  Block *currentBlock; /* current block */
  Tube *tube;          /* current tube */
};

extern Game currentGame;
extern Timer mainTimer;
extern int screenSize[2];

extern int isMenuOverlay;
extern int isGameOn;
extern int isHighScoreMode;
extern int isScoreTableMode;
extern int showSidebar;
extern int showSidebarTextures;
extern float menuDelay;
extern float menuStartTime; 


void gameIdle(void);
void gameDisplay(void);
void scoreDisplay(void);
void initScoreCounter(int n, int m);
void gameReshape(int width, int height);
void initGameGraphics(void);
void initGameBlocks(int nForms, int *selected);
void newBlock (void);
void exitGameMode(void);
void continueGame(void);
void initGame(void);
void startGame(void);
void setGameFunctions(void);
void setGameProjection(void);
void endGame (void);

#endif
