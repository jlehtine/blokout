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

#include <GL/gl.h>
#include <GL/glut.h>
#include "timing.h"
#include "blocks.h"
#include "tube.h"
#include "collision.h"
#include "transforms.h"
#include "control.h"
#include "timing.h"
#include "game.h"
#include "menu.h"

extern int screenSize[2];

/* For viewpoint movement */
int isViewChanged = 0;
int isViewMoving = 0;
int lastMousePosition[2];

void ctrlKeyboard(unsigned char key, int x, int y)
{
  float nowTime;

  glutPostRedisplay();
  nowTime = readTimer(&(currentGame.currentBlock->timer));
  switch(key) {
  case 'd':
  case 'D':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 2, -1, nowTime);
    break;
  case 'e':
  case 'E':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 2, 1, nowTime);
    break;
  case 'a':
  case 'A':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 0, -1, nowTime);
    break;
  case 'q':
  case 'Q':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 0, 1, nowTime);
    break;
  case 's':
  case 'S':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 1, -1, nowTime);
    break;
  case 'w':
  case 'W':
    tryTurnBlock(currentGame.tube, currentGame.currentBlock, 1, 1, nowTime);
    break;
  case ' ':
    currentGame.currentBlock->lastStop = nowTime;
    currentGame.currentBlock->stopHeight =
      currentGame.currentBlock->position[2];
    currentGame.currentBlock->fallSpeed =
      currentGame.tube->form->dimensions[2] / 2;
    break;
  case 'x':
  case 'X':
    exit(0);
    break;
  case 27:
  case 'm':
  case 'M':
    if (isGameOn) {
      /* view-angle might be wrong so we need to correct it */
      currentGame.tube->viewAngle[0] = 0;
      currentGame.tube->viewAngle[1] = 0;
      isViewChanged = 1;
    }
    menuMode();
    break;
  case 't':
  case 'T':
    if (showSidebarTextures)
      showSidebarTextures = 0;
    else 
      showSidebarTextures = 1;
    initMenu();
    break;
  case 'y':
  case 'Y':
    if (showSidebar)
      showSidebar = 0;
    else 
      showSidebar = 1;
    initMenu();
    break;
    
  };
   
}

void ctrlSpecial(int key, int x, int y)
{
  float nowTime;

  glutPostRedisplay();
  nowTime = readTimer(&(currentGame.currentBlock->timer));
  switch(key) {
  case GLUT_KEY_LEFT: /* Move block left */
    tryMoveBlock(currentGame.tube, currentGame.currentBlock, 0, -1, nowTime);
    break;
  case GLUT_KEY_RIGHT: /* Move block right */
    tryMoveBlock(currentGame.tube, currentGame.currentBlock, 0, 1, nowTime);
    break;
  case GLUT_KEY_UP: /* Move block up */
    tryMoveBlock(currentGame.tube, currentGame.currentBlock, 1, 1, nowTime);
    break;
  case GLUT_KEY_DOWN: /* Move block down */
    tryMoveBlock(currentGame.tube, currentGame.currentBlock, 1, -1, nowTime);
    break;
  case GLUT_KEY_F1: /* Default view */
    if(!isViewMoving) {
      currentGame.tube->viewAngle[0] = 0;
      currentGame.tube->viewAngle[1] = 0;
      isViewChanged = 1;
    }
    break;
  case GLUT_KEY_F2: /* 45 degree angle view */
    if(!isViewMoving) {
      currentGame.tube->viewAngle[0] = 0;
      currentGame.tube->viewAngle[1] = PI/4;
      isViewChanged = 1;
    }
    break;
  case GLUT_KEY_F3: /* Side view */
    if(!isViewMoving) {
      currentGame.tube->viewAngle[0] = 0;
      currentGame.tube->viewAngle[1] = PI/2;
      isViewChanged = 1;
    }
    break;
  }
}

void ctrlMouse(int button, int state, int x, int y)
{
  /* Only move viewpoint with left button */
  if(button != GLUT_LEFT_BUTTON)
    return;

  /* Start or stop the viewpoint movement */
  if(state == GLUT_DOWN && !isViewMoving) {
    isViewMoving = 1;
    lastMousePosition[0] = x;
    lastMousePosition[1] = y;
  } else if(state == GLUT_UP)
    isViewMoving = 0;
}

void ctrlMotion(int x, int y)
{
  /* Move viewpoint if necessary */
  if(isViewMoving) {
    currentGame.tube->viewAngle[0] +=
      (x - lastMousePosition[0])/(screenSize[0]/2.0);
    currentGame.tube->viewAngle[1] +=
      (y - lastMousePosition[1])/(screenSize[1]/2.0);
    if(currentGame.tube->viewAngle[1] < 0)
      currentGame.tube->viewAngle[1] = 0;
    if(currentGame.tube->viewAngle[1] > PI/2)
      currentGame.tube->viewAngle[1] = PI/2;
    isViewChanged = 1;
    lastMousePosition[0] = x;
    lastMousePosition[1] = y;
  }
}
