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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#if HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#if HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif
#include "select.h"
#include "transforms.h"
#include "timing.h"
#include "menu.h"
#include "game.h"
#include "overlay.h"

SelectSphere sphere;

void initSphere()
{
  int remainingBlocks, height;
  int position = 0;
  float rotation[3];
  int i;
  Block *cb;
  
  sphere.radius = 15;

  /* initiate viewing attributes */
  sphere.lookPos[0] = sphere.lookPos[1] = 0; sphere.lookPos[2] = 0;
  sphere.lookAt[0] = sphere.lookAt[1] = 0;
  sphere.lookAt[2] = sphere.radius;
  sphere.lookAngle[0] = 0;
  sphere.lookAngle[1] = 0;
  sphere.turnSpeed = 0.4;
  sphere.currentLookAt[0] = sphere.currentLookAt[1] = 0;
  sphere.nextLookAt[0] = sphere.nextLookAt[1] = 0;
  sphere.isViewMoving = 0;
  sphere.isViewUpdated = 1;
  sphere.numSelected = numForms;

  /* initiate lightning */

#if 0
/*  glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, &spotExponent);   */
  glLightfv(GL_LIGHT0, GL_POSITION, spotPosition); 
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);   
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 70.0);   
  glEnable(GL_LIGHT0);
#endif
  
  /* count the number of blocks in each circle */
  if ((numForms - (numForms / 3)) % 2)
    sphere.blocksPerLevel[0] = (numForms / 3) - 1;
  else
    sphere.blocksPerLevel[0] = (numForms / 3);
  remainingBlocks = numForms - sphere.blocksPerLevel[0];
  if ((remainingBlocks / 6) % 2)
    sphere.blocksPerLevel[2] = (remainingBlocks / 6) + 1;
  else
    sphere.blocksPerLevel[2] = remainingBlocks / 6;
  sphere.blocksPerLevel[1] = ((remainingBlocks / 2) -
                              sphere.blocksPerLevel[2] - 1);
  sphere.blocksPerLevel[3] = 1;
  
  /* init memory for the list of blocks */
  sphere.blocks = malloc(sizeof(Block *) * numForms);
  if(sphere.blocks == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  
  /* init memory for the selections */
  sphere.isSelected = malloc(sizeof(int) * numForms);
  if(sphere.isSelected == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  rotation[2] = 0;
  height = -3;
  sphere.curBlock = 0;
  /* count the number of the first block we are looking at */
  for (i = -3; i < 0; i++)
    sphere.curBlock += sphere.blocksPerLevel[abs(i)];

  for(i = 0; i < numForms; i++) {
    float or[3][3], or2[3][3];
    float pos[3], pos2[3];
    
    /* select the block */
    sphere.isSelected[i]=1;

    /* create the (solid) block */
    cb = createBlock(forms[i], 1);
    sphere.blocks[i] = cb;
    if (position >= sphere.blocksPerLevel[abs(height)]) {
      position = 0;
      height++;
    }

    /* first add the normal block */
    pos[0] = 0;
    pos[1] = 0;
    pos[2] = sphere.radius;
    rotation[1] = 2.0 * PI * position / sphere.blocksPerLevel[abs(height)];
    rotation[0] = 0;
    createRotation(rotation, or);
    rotation[0] = PI * height / 6.0;
    rotation[1] = 0;
    createRotation(rotation, or2);
    rotateRotation(or2, or, cb->orientation);
    rotateVector(pos, cb->orientation, pos2);
    cb->position[0] = pos2[0];
    cb->position[1] = pos2[1];
    cb->position[2] = pos2[2];
    
    position++;
  }
}

/*static float rotation; */

void updateSelectView()
{
  int i,j = 0;
  float *l; /* where we were looking at */

  l = sphere.lookAngle;
  
  /* browse through Turns... */
  if (sphere.isViewMoving) {
    float timeNow;
    float turnTime;

    sphere.isViewUpdated = 1;
    timeNow = readTimer(&mainTimer);
    /* the initial position */
    l[0] = 2.0 * PI * sphere.currentLookAt[0] /
      sphere.blocksPerLevel[abs(sphere.currentLookAt[1])];
    l[1] = PI * sphere.currentLookAt[1] / 6.0;
    turnTime = 1.0 / sphere.turnSpeed;

    /* remove finished turns */
    for (j = 0; timeNow - sphere.turns[j].startTime > turnTime &&
           j < sphere.numTurns; j++) {
      TurnInfo *turn;
      
      turn = sphere.turns + j;
      l[turn->axis] += turn->angle;
      if(turn->axis) {
        sphere.currentLookAt[turn->axis] += turn->sign;
        sphere.currentLookAt[0] = sphere.currentLookAt[0] *
          sphere.blocksPerLevel[abs(sphere.currentLookAt[1])] /
          sphere.blocksPerLevel[abs(sphere.currentLookAt[1]-turn->sign)]; 
      }  
      else {
        sphere.currentLookAt[0] = (sphere.currentLookAt[0] + turn->sign +
          sphere.blocksPerLevel[abs(sphere.currentLookAt[1])]) %
          sphere.blocksPerLevel[abs(sphere.currentLookAt[1])];
      } 
                                     }

    for (i = j; i < sphere.numTurns; i++) {
      TurnInfo *turn;
      float moveTime;

      turn = sphere.turns + i;
      moveTime = (timeNow - turn->startTime) * PI / turnTime;
      l[turn->axis] += turn->angle * (1 + sin(-PI/2 + moveTime)) / 2;  
    }

    /* free space from completed moves */
    if(j > 0) {
      sphere.numTurns -= j;
      memmove(sphere.turns,
              sphere.turns + j,
              sphere.numTurns * sizeof(TurnInfo));
    }
    
    if (sphere.numTurns == 0)
      sphere.isViewMoving = 0; 
  }

  glutPostRedisplay();
}

GLfloat tp[3] = {-0.9, -0.9, -1.8}, textPosition[3];

void drawSelectView()
{
  GLfloat m[4][4];
  int i, j, n;

  if (sphere.isViewUpdated) {
    float or[3][3], or2[3][3], or3[3][3];
    float rot[3] = {0, 0, 0};
    float pos[3] = {0, 0, 0};

/*    glDisable(GL_LIGHT0); */
    
    pos[2] = -sphere.radius;
    rot[0] = - sphere.lookAngle[1];
    createRotation(rot, or);
    rot[0] = 0;
    rot[1] = - sphere.lookAngle[0];
    createRotation(rot, or2);
    rotateRotation(or, or2, or3);
    rotateVector(pos, or3, sphere.lookAt);
    rotateVector(tp, or3, textPosition); 
    rot[0] = PI/2 - sphere.lookAngle[1];
    rot[1] = 0;
    createRotation(rot, or);
    rot[0] = 0;
    rot[1] = - sphere.lookAngle[0];
    createRotation(rot, or2);
    rotateRotation(or, or2, or3);
    rotateVector(pos, or3, rot);  
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    glFrustum( -1.0, 1.333, -1.0, 1.0, 1.7, 50.0 );
    /* we are at the origin */ 
    gluLookAt(sphere.lookPos[0], sphere.lookPos[1], sphere.lookPos[2], 
              /* where we are looking at */
              sphere.lookAt[0],sphere.lookAt[1],sphere.lookAt[2],
              /* up is always up */
              rot[0], rot[1], rot[2]); 
    
    glMatrixMode( GL_MODELVIEW );
    
    sphere.isViewUpdated = 0;
  }
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (n = 0; n < numForms; n++) {
    glLoadIdentity();
    for(i=0; i<3; i++)
      for(j=0; j<3; j++)
        m[i][j] = sphere.blocks[n]->orientation[i][j]; 
    for(i=0; i<3; i++)
      m[i][3] = 0;
    m[3][0] = sphere.blocks[n]->position[0];
    m[3][1] = sphere.blocks[n]->position[1];
    m[3][2] = sphere.blocks[n]->position[2];
    m[3][3] = 1;
    glMultMatrixf((const GLfloat *)m);
    drawBlock(sphere.blocks[n]);
  }

  if (!sphere.isViewMoving) {
    char *s;

    begin2DOverlay(screenSize[0], screenSize[1]);
    if (sphere.isSelected[sphere.curBlock])
      s = "block selected";
    else
      s = "block not selected";
    glColor3f(1, 1, 1);
    drawText(GLUT_BITMAP_TIMES_ROMAN_24,
             screenSize[0]/10, screenSize[1]/10, s);
    end2DOverlay();
  }
  
  glutSwapBuffers();
}

void turnSphere(int axis, int sign, float nowTime)
{
  TurnInfo *turn;
  float curAngle=0, newAngle=0;
  int diff;
    
  /* reser ve memory for turns */
  if(sphere.maxTurns == 0) {
    sphere.turns = malloc(sizeof(TurnInfo) * 4);
    assert(sphere.turns != NULL);
    sphere.maxTurns = 4;
  } else if(sphere.numTurns + 2 >= sphere.maxTurns) {
    sphere.maxTurns *= 2;
    sphere.turns = realloc(sphere.turns, sizeof(TurnInfo) * sphere.maxTurns);
    assert(sphere.turns != NULL);
  }

  if (axis) {
    diff = sphere.nextLookAt[0];
    curAngle = 2.0 * PI * sphere.nextLookAt[0] /
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1])];
    sphere.nextLookAt[1] += sign;
    sphere.nextLookAt[0] = sphere.nextLookAt[0] *
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1])] /
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1] - sign)];
    diff = sphere.nextLookAt[0] - diff;
    newAngle = 2.0 * PI * sphere.nextLookAt[0] /
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1])];
  }
  else
    sphere.nextLookAt[0] = (sphere.blocksPerLevel[abs(sphere.nextLookAt[1])] +
                            sphere.nextLookAt[0] + sign) %
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1])];

  /* Initialize turn info */
  turn = sphere.turns + sphere.numTurns;
  turn->startTime = nowTime;
  turn->axis = axis;
  turn->sign = sign;
  if (!axis)
    turn->angle = sign * 2.0 * PI /
      sphere.blocksPerLevel[abs(sphere.nextLookAt[1])];
  else
    turn->angle = sign * PI / 6.0;

  /* Increase turn count and continue */
  sphere.numTurns++;
  if (curAngle != newAngle) {
    turn = sphere.turns + sphere.numTurns;
    turn->startTime = nowTime;
    turn->axis = 0;
    turn->angle = newAngle - curAngle;
    turn->sign = 0;   
    
    /* Increase turn count and continue */
    sphere.numTurns++;
  }  
  
  sphere.isViewMoving = 1;
}

void selectKeyboard(unsigned char key, int x, int y)
{
  glutPostRedisplay();
  switch(key) {
  case 's':
  case 'S':
  case 13:
    if (sphere.isSelected[sphere.curBlock]){
      sphere.isSelected[sphere.curBlock] = 0;
      sphere.numSelected--;
    }
    else {
      sphere.isSelected[sphere.curBlock] = 1;
      sphere.numSelected++;
    }
    break;
  case 27:
  case 'm':
  case 'M':
    initGameBlocks(sphere.numSelected, sphere.isSelected);
    setGameFunctions();
    setGameProjection();
    menuMode();
    break;
  case 'n':
  case 'N':
    initGameBlocks(sphere.numSelected, sphere.isSelected);
    startGame();
    break;
  case 'x':
  case 'X':
    exit(0);
    break;
  };
}

void selectSpecial(int key, int x, int y)
{
  float timeNow;
  int i;

  timeNow = readTimer(&mainTimer);
  switch(key) {
  case GLUT_KEY_LEFT: 
    turnSphere(0,-1, timeNow);
    if (sphere.nextLookAt[0] ==
        sphere.blocksPerLevel[abs(sphere.nextLookAt[1])] - 1)
      sphere.curBlock += sphere.nextLookAt[0];
    else
      sphere.curBlock--;
    break;
  case GLUT_KEY_RIGHT: 
    turnSphere(0,1, timeNow);
    if (sphere.nextLookAt[0] == 0)
      sphere.curBlock -= sphere.blocksPerLevel[abs(sphere.nextLookAt[1])] - 1;
    else
      sphere.curBlock++;
    break;
  case GLUT_KEY_UP: 
    if (sphere.nextLookAt[1] > -3) {
      sphere.curBlock = 0;      
      turnSphere(1,-1, timeNow);
      for(i = -3; i < sphere.nextLookAt[1]; i++)
        sphere.curBlock += sphere.blocksPerLevel[abs(i)];
      sphere.curBlock += sphere.nextLookAt[0];
    }
    break;
  case GLUT_KEY_DOWN: 
    if (sphere.nextLookAt[1] < 3) {
      turnSphere(1,1, timeNow);
      sphere.curBlock = 0;
      for(i = -3; i < sphere.nextLookAt[1]; i++)
        sphere.curBlock += sphere.blocksPerLevel[abs(i)];
      sphere.curBlock += sphere.nextLookAt[0];
    }
    break;
  }
}

void exitSelectMode()
{
  int i;
  
  free(sphere.turns);
  free(sphere.isSelected);
  for(i=0; i<numForms; i++)
    deleteBlock(sphere.blocks[i]);
  free(sphere.blocks);
}

void selectMode()
{
  initSphere();
  updateSelectView();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glFrustum(-1.0, 1.333, -1.0, 1.0, 1.7, 50.0);
  gluLookAt(sphere.lookPos[0], sphere.lookPos[1], sphere.lookPos[2], 
            /* where we are looking at */
            sphere.lookAt[0],sphere.lookAt[1],sphere.lookAt[2],
            /* up is here */
            0,1,0);
  
  glMatrixMode(GL_MODELVIEW);
  drawSelectView();

  glutIdleFunc(updateSelectView);
  glutDisplayFunc(drawSelectView);
  glutKeyboardFunc(selectKeyboard);
  glutSpecialFunc(selectSpecial);
}
