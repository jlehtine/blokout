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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glut.h>
#include "blocks.h"
#include "texture.h"
#include "transforms.h"
#include "counter.h"
#include "game.h"

#define HALFTURN PI/10

Counter *newCounter()
{
  Counter *c;
  int i, j;

  GLfloat mat[4] = {1.0, 0.96, 0.8, 1.0};

  c = malloc(sizeof(Counter));
  assert (c != NULL);

  /* initialize constant values */
  c->value = 0;
  c->currentAngle = c->angle = c->diff = 0;
  c->isRotating = 0;
  c->position[0] = c->position[1] = c->position[2] = 0.0;
  c->numTurns = c->maxTurns = 0;
  c->turns = NULL;
  c->turnSpeed = 0.4;

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      c->orientation[i][j] = 0;

  /* Initialize counter display list */
  c->displayListId = glGenLists(1);
  glNewList(c->displayListId, GL_COMPILE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImgs[0]->sizeX, 
               texImgs[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
               (GLvoid *) texImgs[0]->data);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glColor3f(mat[0], mat[1], mat[2]);

  /* draw the sides */
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(sin(HALFTURN), 0.0, 0.0);
  for (i = 10; i >= 0; i--) {
    float angle = i * PI / 5;

    glVertex3f(sin(HALFTURN), sin(angle - HALFTURN),
               cos(angle - HALFTURN));
  }
  glEnd();

  /* ...and the other */
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(-sin(HALFTURN), 0.0, 0.0);
  for (i = 0; i < 11; i++) {
    float angle = i * PI / 5;

    glVertex3f(-sin(HALFTURN), sin(angle - HALFTURN),
               cos(angle - HALFTURN));
  }
  glEnd();

  /* Draw the counter faces */
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUAD_STRIP);
  for (i = 10; i >= 0; i--) {
    float angle = i * PI / 5;
    glNormal3f(0.0, sin(angle), cos(angle)); 
    
    /* draw the couple of points */
    glTexCoord2f(1.0, i / 10.0);
    glVertex3f(sin(HALFTURN), sin(angle - HALFTURN), cos(angle - HALFTURN));
    glTexCoord2f(0.0, i /10.0);
    glVertex3f(-sin(HALFTURN), sin(angle - HALFTURN), cos(angle - HALFTURN));
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEndList();
  return c;
}

void deleteCounter(Counter *counter)
{
  glDeleteLists(counter->displayListId, 1);
  free(counter);
}

void drawCounter(Counter *counter)
{
  /* Rotate the counter around x-axis and draw it */
  glPushMatrix();
  glRotatef(-counter->currentAngle*180.0/PI, 1, 0, 0);
  glCallList(counter->displayListId);
  glPopMatrix();
}

int addCounter(Counter *counter, int amount) {

  int rv;
  float timeNow;
  TurnInfo *turn;
  
  rv = amount / 10;
  if (counter->value + (amount % 10) > 9)
    rv++;
  counter->value = (counter->value + amount) % 10;
  counter->isRotating = 1;

  timeNow = readTimer(&mainTimer);
    
  /* reserve memory for turns */
  if(counter->maxTurns == 0) {
    counter->turns = malloc(sizeof(TurnInfo) * 4);
    assert(counter->turns != NULL);
    counter->maxTurns = 4;
  }
  /* we should never need this many turns, but in case we do->.. */
  else if(counter->numTurns +1 >= counter->maxTurns) {
    counter->maxTurns *= 2;
    counter->turns = realloc(counter->turns, sizeof(TurnInfo)*counter->maxTurns);
    assert(counter->turns != NULL);
  }


  /* Initialize turn info */
  turn = counter->turns + counter->numTurns;
  turn->startTime = timeNow;
  turn->axis = 0;
  turn->sign = -1;
  if(counter->numTurns == 0) {
    float diff = HALFTURN/4 * sin(rand() * PI * 2 / RAND_MAX);

    turn->angle = - (amount % 10) * PI/5 - counter->diff - diff;
    counter->diff = diff;
  }
  else
    turn->angle = - (amount % 10) * PI/5;
  /* Increase turn count and continue */
  counter->numTurns++;

  return rv;
}

void updateCounter(Counter *counter) {

  int i, j;
  
  if (counter->isRotating) {
    float angle;
    float timeNow;
    float turnTime;

    timeNow = readTimer(&mainTimer);
    /* the initial position */
    angle = counter->angle;
    turnTime = 1.0 / counter->turnSpeed;

    /* remove finished turns */
    for (j = 0; timeNow - counter->turns[j].startTime > turnTime &&
           j < counter->numTurns; j++) {
      TurnInfo *turn;
      
      turn = counter->turns + j;
      angle += turn->angle;
      counter->angle += turn->angle;
    }

    for (i = j; i < counter->numTurns; i++) {
      TurnInfo *turn;
      float moveTime;
      
      turn = counter->turns + i;
      moveTime = (timeNow - turn->startTime) * PI / turnTime;
      angle += turn->angle * (1 + sin(-PI/2 + moveTime)) / 2;  
    }
    if(j > 0) {
      counter->numTurns -= j;
      memmove(counter->turns,
              counter->turns + j,
              counter->numTurns * sizeof(TurnInfo));
    }
    
    counter->currentAngle = angle;
    
    if (counter->numTurns == 0)
      counter->isRotating = 0; 
  }
}
