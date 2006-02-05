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
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include "overlay.h"
#include "menu.h"
#include "transforms.h"
#include "select.h"
#include "game.h"
#include "blocks.h"
#include "highscoreui.h"

int numMenus = 3;
int currentMenu = 0;
int selection = 1;

static const char *menus[][22] = {
  {"MAIN MENU",
   "Help",
   "Settings",
   "High Score Table",
   "Select Blocks",
   "Continue Game",
   "End Game",
   "New Game",
   "Quit",
   NULL},
  {"HELP",
   "------- Controls in game ---------",
   "e,d  - Rotate block around x-axis",
   "w,s  - Rotate block around y-axis",
   "q,a  - Rotate block around z-axis",
   "F1-F3 - Select view",
   "t - set sidebar textures on/off",
   "y - show / hide sidebar",
   "arrows - Move block",
   "",
   "----- Controls in block selection ----",
   "arrows - Rotate sphere (select block)",
   "s / ENTER - select/unselect block",
   "",
   "---- Main Menu / general keys ----",
   "h         - help (this) menu",
   "s         - settings menu",
   "b         - block selection",
   "n         - new game",
   "m or ESC  - return to main menu",
   "x         - exit game",
   NULL},
  {"SETTINGS",
   "Level",
   "Height",
   "X-width",
   "Y-width",
   "Solid blocks",
   "Background Colors",
   "Sidebar",
   "Sidebar textures",
   NULL}};

char *settingSelected[9];


static char *onoff[] = {"OFF", "ON"};
static char *level[] = {"0","1","2","3","4","5","6","7","8","9","10","11",
                        "12","13","14","15","16","17","18","19","20"};

void initMenu(void)
{
#ifdef DEBUG
  fprintf(stderr, "initMenu()\n");
#endif
  settingSelected[0] = level[currentGame.startLevel];
  settingSelected[1] = level[currentGame.height];
  settingSelected[2] = level[currentGame.xWidth];
  settingSelected[3] = level[currentGame.yWidth];  
  settingSelected[4] = onoff[currentGame.solidBlocks];
  settingSelected[5] = onoff[currentGame.backgroundColor];
  settingSelected[6] = onoff[showSidebar];
  settingSelected[7] = onoff[showSidebarTextures];
}
  
void menuAction(int selection)
{
#ifdef DEBUG
  fprintf(stderr, "menuAction(%d)\n", selection);
#endif

  if (currentMenu == MAIN_MENU) {
    if (selection < numMenus)
      currentMenu = selection;
    else
      switch(selection) {
      case HIGH_SCORE:
        isScoreTableMode = 1;
        isMenuOverlay = 0;
        break;
      case BLOCK_SELECT:
        if (!isGameOn)
          selectMode();
        break;
      case CONTINUE_GAME:
        if(isGameOn)
          continueGame();
        break;
      case END_GAME:
        if(isGameOn)
          endGame();
        break;
      case NEW_GAME:
        isMenuOverlay = 0;
        startGame();
        break;
      case QUIT: 
        glutDestroyWindow(glutGetWindow());
        exit(0);
        printf("Byeeebyeee!\n");
        break;
      default:
        break;
      }
  }
  else if (currentMenu == SETTINGS_MENU && (!isGameOn || selection > YWIDTH)) {
    switch (selection){
    case LEVEL:
      currentGame.startLevel = ++currentGame.startLevel % 10;
      initMenu();
      break;
    case HEIGHT:
      currentGame.height = (++currentGame.height - 5) % 16 + 5;
      initMenu();
      break;
    case XWIDTH:
      currentGame.xWidth = (++currentGame.xWidth - 3) % 5 + 3;
      initMenu();
      break;
    case YWIDTH:
      currentGame.yWidth = (++currentGame.yWidth - 3) % 5 + 3;
      initMenu();
      break;    
    case SOLID_BLOCKS:
      currentGame.solidBlocks = ++currentGame.solidBlocks % 2;
      deleteBlockDisplayList(currentGame.currentBlock);
      createBlockDisplayList(currentGame.currentBlock,
                             currentGame.solidBlocks);
      initMenu();
      break;
    case BG_COLOR:
      currentGame.backgroundColor = ++currentGame.backgroundColor % 2;
      initMenu();
      break;
    case SIDEBAR:
      showSidebar = ++showSidebar % 2;
      initMenu();
      break;
    case SIDEBAR_TEXTURES:
      showSidebarTextures = ++showSidebarTextures % 2;
      initMenu();
      break;
    };
  }
}

void drawMenuOverlay(int menuNo, int selection)
{
  int i;
  GLfloat mat[4]={0.0,1.0,0.0,1.0};
  GLfloat bgmat[4]={0.5,0.5,0.5,0.5};
  void *font;
  int widest;
  char **menu = (char **)menus[menuNo];
  
#ifdef DEBUG
  fprintf(stderr, "printMenu(%d, %d)\n", menuNo, selection);
#endif

  begin2DOverlay(screenSize[0], screenSize[1]);

  /* Draw the background */
  glEnable(GL_BLEND);
  glColor4f(bgmat[0], bgmat[1], bgmat[2], bgmat[3]);
  drawRectangle(screenSize[0]/20, screenSize[0]*19/20,
                screenSize[1]/20, screenSize[1]*19/20);
  
  /* Draw selection bar */
  if (menuNo != HELP_MENU) {
    glColor4f(1, 0, 0, 0.5);
    drawRectangle(screenSize[0]/10, screenSize[0]*9/10,
                  screenSize[1]*8/10 - 30*(selection-1) - 10,
                  screenSize[1]*8/10 - 30*(selection-2) - 10);
  }
  glDisable(GL_BLEND);

  /* Draw menu header */
  glColor3f(mat[0], mat[1], mat[2]);
  drawText(GLUT_BITMAP_TIMES_ROMAN_24,
           (screenSize[0] - getTextWidth(GLUT_BITMAP_TIMES_ROMAN_24,
                                         menu[0]))/2,
           screenSize[1]*19/20 - 40,
           menu[0]);
  
  /* Select menu font */
  font = GLUT_BITMAP_HELVETICA_18;
  if(menuNo == HELP_MENU)
    font = GLUT_BITMAP_HELVETICA_12;

  /* Draw menu selections */
  widest = 0;
  for(i=1; menu[i] != NULL; i++) {
    /* Choose color */
    glColor3f(mat[0], mat[1], mat[2]);
    if(menuNo == MAIN_MENU)
      if((isGameOn && i == BLOCK_SELECT) ||
         (!isGameOn && (i == CONTINUE_GAME || i == END_GAME)))
        glColor3f(mat[0]/2, mat[1]/2, mat[2]/2);
    if(menuNo == SETTINGS_MENU)
      if(isGameOn && i <= YWIDTH)
        glColor3f(mat[0]/2, mat[1]/2, mat[2]/2);
    
    /* Draw selection text */
    drawText(font, screenSize[0]/10 + 40,
             screenSize[1]*8/10 - (menuNo == HELP_MENU ? 18 : 30)*(i-1),
             menu[i]);

    /* Draw selection value */
    if(menuNo == SETTINGS_MENU) {
      int j;
      
      j = getTextWidth(font, menu[i]);
      if(j > widest)
        widest = j;
    }
  }

  /* Draw selection values */
  if(menuNo == SETTINGS_MENU)
    for(i=1; menu[i] != NULL; i++)
      drawText(font, screenSize[0]/10 + 70 + widest,
               screenSize[1]*8/10 - 30*(i-1),
               settingSelected[i-1]);
  
  end2DOverlay();
}


void menuSpecial(int key, int x, int y)
{
  glutPostRedisplay();

  if(isHighScoreMode) {
    if(highScoreKeyboard(key)) {
      isHighScoreMode = 0;
      isScoreTableMode = 1;
    }
    return;
  }

  if(isScoreTableMode) {
    isScoreTableMode = 0;
    isMenuOverlay = 1;
    return;
  }

  switch(key) {
  case GLUT_KEY_RIGHT:
    menuAction(selection);
    break;
  case GLUT_KEY_LEFT:
    currentMenu=0;
    selection=1;
    break;
  case GLUT_KEY_UP:
    if (selection > 1)
      selection--;
    break;
  case GLUT_KEY_DOWN:
    if(menus[currentMenu][selection+1] != NULL)
      selection++;
    break;
  }
}

void menuKeyboard(unsigned char key, int x, int y)
{
  glutPostRedisplay();
  
  if(isHighScoreMode) {
    if(highScoreKeyboard(key)) {
      isHighScoreMode = 0;
      isScoreTableMode = 1;
    }
    return;
  }

  if(isScoreTableMode) {
    isScoreTableMode = 0;
    isMenuOverlay = 1;
    return;
  }
  
  switch(key) {
  case 'h':
  case 'H':
    currentMenu=HELP_MENU;
    break;
  case 's':
  case 'S':
    currentMenu=SETTINGS_MENU;
    break;
  case 'x':
  case 'X':
    exit(0);
    break;
  case 'b':
  case 'B':
    if (!isGameOn)
      selectMode();
    break;
  case 'c':
  case 'C':
    continueGame();
    break;
  case 'n':
  case 'N':
    startGame();
    break;
  case 27:
  case 'm':
  case 'M':
    currentMenu=MAIN_MENU;
    break;
  case 13:
    menuAction(selection);
    break;    
  };
}

void menuMode (void) {

#ifdef DEBUG
  fprintf(stderr, "menuMode()\n");
#endif

  /* initiate selections */
  currentMenu = 0;
  selection = 1;

  /* draw menu and read timer */
  isMenuOverlay = 1;
  menuStartTime = readTimer(&mainTimer);

  /* set key-handling  routines */
  glutKeyboardFunc(menuKeyboard);
  glutSpecialFunc(menuSpecial);
  glutIdleFunc(NULL);
}
