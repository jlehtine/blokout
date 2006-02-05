/*************************************************************************
 * Copyright (C) 1998-2000 Johannes Lehtinen
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
#include <assert.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "tube.h"
#include "blocks.h"
#include "collision.h"
#include "timing.h"
#include "menu.h"
#include "transforms.h"
#include "control.h"
#include "game.h"
#include "forms.h"
#include "texture.h"
#include "counter.h"
#include "highscore.h"
#include "highscoreui.h"
#include "overlay.h"

#define SC_XPOS 11.5
#define SC_XWIDTH -0.8
#define SC_YPOS 7.0
#define SC_ZPOS -16.0

int screenSize[2];
int isMenuOverlay = 0;
int isGameOn = 0;
int isHighScoreMode = 0;
int isScoreTableMode = 0;
int showSidebar = 1;
int showSidebarTextures = 1;
float menuDelay = 0;
float menuStartTime; 

Game currentGame;
Timer mainTimer;

int numberOfCounters;
Counter **scoreCounter;

int numberOfCubeCounters;
Counter **cubeCounter;

Counter *levelCounter;

/* Set game projection */
void setGameProjection(void)
{
  float viewPos[3];
  Tube *tube = currentGame.tube;

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
    
  glFrustum( -1.0, 1.333, -1.0, 1.0, 1.7,
             tube->form->dimensions[2] +
             (tube->form->dimensions[0] > tube->form->dimensions[1] ?
              tube->form->dimensions[0] : tube->form->dimensions[1])
             + 1);
  viewPos[0] = tube->form->dimensions[2] * cos(tube->viewAngle[0] - PI/2) *
    sin(tube->viewAngle[1]);
  viewPos[1] = tube->form->dimensions[2] * sin(tube->viewAngle[0] - PI/2) *
    sin(tube->viewAngle[1]);
  viewPos[2] = (-tube->viewPoint[2]) *
    (sin(tube->viewAngle[1] + PI/2) - 1) - 1.7;
  gluLookAt(tube->form->dimensions[0] / 2.0 + viewPos[0],
            tube->form->dimensions[1] / 2.0 + viewPos[1],
            viewPos[2],
            /* where we are looking at */
            tube->viewPoint[0],
            tube->viewPoint[1],
            tube->viewPoint[2],
            /* up is here... */
            cos(tube->viewAngle[0]+PI/2),
            sin(tube->viewAngle[0]+PI/2),
            1);
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

void gameIdle(void)
{
  float timeNow;
  Tube *tube = currentGame.tube;
  Block *currentBlock = currentGame.currentBlock;

#ifdef DEBUG
  fprintf(stderr, "gameIdle()\n");
#endif
  
  if(isViewChanged) {
    setGameProjection();
    isViewChanged=0;
  }
  if (!isMenuOverlay && !isHighScoreMode && !isScoreTableMode) {
    timeNow = readTimer(&(currentBlock->timer));
    updateBlock(currentBlock, timeNow);
    tryLowerBlock(tube, currentBlock, timeNow);
    if(timeNow - currentBlock->lastFall > 1) {
      addBlockToTube(tube, currentBlock);
      if (tube->height < tube->form->dimensions[2]-5)
        newBlock();
      else
        endGame();
    }
    glutPostRedisplay();
  }
}


/* function to draw the screen */
void gameDisplay( void )
{
  GLfloat m[4][4];
  int i, j;
  
#ifdef DEBUG
  fprintf(stderr, "gameDisplay()\n");
#endif
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  if (currentGame.backgroundColor)
    drawTubeWalls(currentGame.tube);
  drawTubeGrid(currentGame.tube);
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m[i][j] = currentGame.currentBlock->orientation[i][j];
  for(i=0; i<3; i++)
    m[i][3] = 0;
  m[3][0] = currentGame.currentBlock->position[0] + 0.5;
  m[3][1] = currentGame.currentBlock->position[1] + 0.5;
  m[3][2] = currentGame.currentBlock->position[2] + 0.5;
  m[3][3] = 1;

  drawCubes(currentGame.tube);
  glPushMatrix();
  glMultMatrixf((const GLfloat *)m);
  drawBlock(currentGame.currentBlock);
  glPopMatrix();

  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      m[i][j] = 0;
  if (isMenuOverlay)
    drawMenuOverlay(currentMenu, selection);
  else if (showSidebar)
    scoreDisplay();

  if(isHighScoreMode)
    highScoreDisplay(screenSize);
  if(isScoreTableMode)
    drawScoreTableOverlay(currentGame.scoreTable, screenSize);
  
  glutSwapBuffers();
}

/* Draw score display on the right */
void scoreDisplay()
{
  char s[10];
  int i = 0, j, k;

  begin2DOverlay(screenSize[0], screenSize[1]);
  glEnable(GL_BLEND);
  glColor4f(0.2, 0.2, 1, 0.5);
  drawRectangle(screenSize[0]-150, screenSize[0],
                0, screenSize[1]);
  glDisable(GL_BLEND);
  glColor3f(0.8, 0.8, 0);
  drawText(GLUT_BITMAP_TIMES_ROMAN_24,
           screenSize[0] - 25 -
           getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "SCORE"),
           screenSize[1] - 50,
           "SCORE");
  drawText(GLUT_BITMAP_TIMES_ROMAN_24,
           screenSize[0] - 25 -
           getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "LEVEL"),
           screenSize[1] - 220,
           "LEVEL");
  drawText(GLUT_BITMAP_TIMES_ROMAN_24,
           screenSize[0] - 25 -
           getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "CUBES"),
           screenSize[1] - 370,
           "CUBES");
  if (!showSidebarTextures) {
    if (currentGame.score != 0) {
      j = currentGame.score;
      while(j > 0) {
        i++;
        j = j/10;
      }
      s[i] = '\0';
      for (j = currentGame.score; i > 0; j=j/10, i--)
        s[i-1]= '0' + (j % 10);
    } 
    else {
      s[0] = '0';
      s[1] = '\0';
    }
    drawText(GLUT_BITMAP_TIMES_ROMAN_24,
             screenSize[0] - 25 -
             getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "SCORE"),
             screenSize[1] - 90, s);
    s[0] = '0' + currentGame.level;
    s[1] = '\0';
    drawText(GLUT_BITMAP_TIMES_ROMAN_24,
             screenSize[0] - 25 -
             getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "LEVEL"),
             screenSize[1] - 260,
             s);
    if (currentGame.cubesDropped != 0) {
      j = currentGame.cubesDropped;
      while(j > 0) {
        i++;
        j = j/10;
      }
      s[i] = '\0';
      for (j = currentGame.cubesDropped; i > 0; j=j/10, i--)
        s[i-1]= '0' + (j % 10);
    } 
    else {
      s[0] = '0';
      s[1] = '\0';
    }
    drawText(GLUT_BITMAP_TIMES_ROMAN_24,
             screenSize[0] - 25 -
             getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24, "CUBES"),
             screenSize[1] - 410,
             s);
  }
  end2DOverlay();
  if(showSidebarTextures) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenSize[0], 0, screenSize[1], -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
  
     for (k = 0; k < numberOfCounters; k++) {
      glLoadIdentity();
      glTranslatef(
        screenSize[0] - 20 - k*25,
        screenSize[1] - 100,
        0);
      glRotatef(5, 0, 1, 0);
      glScalef(32, 32, 32);
      updateCounter(scoreCounter[k]);
      drawCounter(scoreCounter[k]);
    }
    
    glLoadIdentity();
    glTranslatef(
      screenSize[0] - 20,
      screenSize[1] - 270,
      0);
    glRotatef(5, 0, 1, 0);
    glScalef(32, 32, 32);
    updateCounter(levelCounter);
    drawCounter(levelCounter);
    
    for (k = 0; k < numberOfCubeCounters; k++) {
      glLoadIdentity();
      glTranslatef(
        screenSize[0] - 20 - k*25,
        screenSize[1] - 430,
        0);
      glRotatef(5, 0, 1, 0);
      glScalef(32, 32, 32);
      updateCounter(cubeCounter[k]);
      drawCounter(cubeCounter[k]);
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
}

void initScoreCounter(int n, int m)
{
  int i;

  levelCounter = newCounter();

  if (scoreCounter != NULL) {
    for (i = 0; i < numberOfCounters; i++) {
      free(scoreCounter[i]->turns);
      free(scoreCounter[i]);
    }
    free(scoreCounter);
  }

  
  numberOfCounters = n;
  scoreCounter = malloc(sizeof(Counter *) * n);
  assert(scoreCounter != NULL);
  for (i = 0; i < n; i++) {
    scoreCounter[i] = newCounter();
  }

  if (cubeCounter != NULL) {
    for (i = 0; i < numberOfCubeCounters; i++) {
      free(cubeCounter[i]->turns);
      free(cubeCounter[i]);
    }
    free(cubeCounter);
  }

  
  numberOfCubeCounters = m;
  cubeCounter = malloc(sizeof(Counter *) * m);
  assert(cubeCounter != NULL);
  for (i = 0; i < m; i++) {
    cubeCounter[i] = newCounter();
  }
}

void gameReshape( int width, int height )
{
#ifdef DEBUG
  fprintf(stderr, "gameReshape(%d, %d)\n", width, height);
#endif
  
  /* initiate the looking point and window */
  glViewport( 0, 0, width, height );
  screenSize[0] = width;
  screenSize[1] = height;
  setGameProjection();
}

static GLfloat lightPosition[4] = { -1.0, 1.0, 1.0, 0.0 };

void initGameGraphics( void )
{
   static GLfloat mat[4] = { 1.0, 1.0, 1.0, 1.0 };

#ifdef DEBUG
  fprintf(stderr, "initGameGraphics()\n");
#endif
  
   /* setup lighting, etc */
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 

   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_NORMALIZE);
   glShadeModel(GL_FLAT);
   glMatrixMode(GL_MODELVIEW);

   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glViewport( 0, 0, screenSize[0], screenSize[1]);

   /* Set game projection */
   setGameProjection();
}

void newBlock () {

  float timeNow;
  int newLevel, i, s, cubes;
  float moveScore;

  /* read the timer */
  timeNow = readTimer(&currentGame.currentBlock->timer);
  /* increase all the values according to the move */
  currentGame.cubesDropped+=currentGame.currentBlock->form->numCubes;
  currentGame.levelsDescended+=currentGame.tube->lastDrop;

  newLevel= currentGame.cubesDropped / 70;
  if (currentGame.level < 10 && currentGame.level < newLevel)
  {
    currentGame.level=newLevel;
    if (currentGame.level < 5)
      currentGame.levelFactor = currentGame.level / 5.0;
    else 
      currentGame.levelFactor = currentGame.level - 5.0;
    addCounter(levelCounter, 1);
  }
  /* update score */
  moveScore = ((currentGame.levelFactor * timeNow * 200 *
                pow(2, currentGame.tube->lastDrop)*
                currentGame.currentBlock->fallSpeed) /
               (currentGame.tube->form->dimensions[2] -
                currentGame.tube->height));
  currentGame.score += (int) (moveScore + 0.5);
  
  for (i = 0, s = (int) (moveScore + 0.5); s > 0; i++)
    s = addCounter(scoreCounter[i],s);
  
  for (i = 0, cubes = currentGame.currentBlock->form->numCubes; cubes > 0; i++)
    cubes = addCounter(cubeCounter[i],cubes);

  currentGame.tube->lastDrop=0;

  deleteBlock(currentGame.currentBlock);
  currentGame.numCurrentBlock = rand() % currentGame.numFormsInGame;
  currentGame.currentBlock = createBlock(
    currentGame.formsInGame[currentGame.numCurrentBlock],
    currentGame.solidBlocks);
  
  /* set the speed of the block */
  currentGame.currentBlock->fallSpeed+=currentGame.level*0.3;
}

void initGameBlocks(int nForms, int *selected)
{
  int i,j;

  if( currentGame.formsInGame != NULL)
    free(currentGame.formsInGame);
  currentGame.formsInGame = malloc(sizeof(Form) * nForms);
  assert(currentGame.formsInGame != NULL);
  currentGame.numFormsInGame = nForms;
  
  for (i = 0, j = 0; i < numForms; i++)
    if (selected[i]) {
      currentGame.formsInGame[j] = forms[i];
      j++;
    }
}

void exitGameMode(void) {
  
  free(currentGame.formsInGame);
  deleteBlock(currentGame.currentBlock);
  free(currentGame.currentBlock);
  deleteTube(currentGame.tube);
}

void continueGame(void) {

  /* remove menu */
  isMenuOverlay = 0;

  /* adjust the clock */
  menuDelay = readTimer(&mainTimer) - menuStartTime;
  currentGame.currentBlock->timer.cumulativeTime -= menuDelay;
  menuDelay = 0;
  
  /* return control */
  setGameFunctions();
}

void endGame () {

#ifdef DEBUG
  fprintf(stderr, "endGame()\n");
#endif

  /* show menu and end the game */
  menuMode();
  isGameOn = 0;

  /* normalize view */
  currentGame.tube->viewAngle[0] = 0;
  currentGame.tube->viewAngle[1] = 0;
  isViewChanged = 1;

  /* Check if the player made it to the high score table */
  if(isHighScore(currentGame.scoreTable, currentGame.score)) {
    isMenuOverlay = 0;
    beginHighScore(currentGame.scoreTable, currentGame.score);
    isHighScoreMode = 1;
  }
  else {
    isMenuOverlay = 0;
    isScoreTableMode = 1;
  }
}

void initGame(void)
{
  /* initialize random seed */
  srand(readTimer(&mainTimer) * 10000);
  
  currentGame.currentBlock = malloc(sizeof(Block));
  if(currentGame.currentBlock == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  /* Initialize variables */
  isViewChanged = 0;
  isViewMoving = 0;
  isGameOn = 0;
  isMenuOverlay = 1;
  isScoreTableMode = 0;
  isHighScoreMode = 0;
  menuStartTime = 0;
  menuDelay = 0;
  currentGame.solidBlocks = 1;
  currentGame.backgroundColor = 1;
  currentGame.startLevel = 2;
  currentGame.levelFactor = 2.0 / 5.0;
  currentGame.height = 18;
  currentGame.xWidth = 3;
  currentGame.yWidth = 3;
  currentGame.level=currentGame.startLevel;
  currentGame.score=0;

  /* randomize and create the first block and create tube */
  currentGame.numCurrentBlock = rand() % currentGame.numFormsInGame;
  currentGame.currentBlock = createBlock(
    currentGame.formsInGame[currentGame.numCurrentBlock],
    currentGame.solidBlocks);
  currentGame.tube=createTube(currentGame.xWidth,currentGame.yWidth,
                              currentGame.height);
  currentGame.scoreTable = newScoreTable();

  /* Select and load highscore file */
  loadScoreTable(currentGame.scoreTable, selectHighScoreFile());

  /* Create score counter */
  initScoreCounter(5,3);

  /* Initialize functions */
  setGameFunctions();
}

void startGame(void)
{
  /* initialize random seed */
  srand(readTimer(&mainTimer) * 10000);
  
  /* Get a new block */
  newBlock();
  
  /* Initialize view controls */
  isViewChanged = 1;
  isViewMoving = 0;

  /* Create score counter */
  initScoreCounter(5, 3);

  /* Initialize variables */
  isGameOn = 1;
  isMenuOverlay = 0;
  menuStartTime = 0;
  menuDelay = 0;
  currentGame.level=currentGame.startLevel;
  currentGame.score=0;
  currentGame.cubesDropped = 0;
  currentGame.levelsDescended = 0;
  addCounter(levelCounter, currentGame.level);

  /* Initialize tube */
  deleteTube(currentGame.tube);
  currentGame.tube=createTube(currentGame.xWidth,currentGame.yWidth,
                              currentGame.height);
  
  /* Initialize functions */
  setGameFunctions();
}

void setGameFunctions() {
  
  glutIdleFunc( gameIdle );
  glutReshapeFunc( gameReshape );
  glutDisplayFunc( gameDisplay ); 
  glutKeyboardFunc( ctrlKeyboard );
  glutSpecialFunc( ctrlSpecial );
  glutMouseFunc(ctrlMouse);
  glutMotionFunc(ctrlMotion); 
}
