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
#include <time.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <GL/gl.h>
#include "blocks.h"
#include "tube.h"
#include "transforms.h"
#include "texture.h"

void createBlockDisplayList(Block *block, int drawSolid)
{
  int i,j; /* indexes */
  BOPolygon *p, *polygons; /* temporary polygons */
  BOVertex *vertices; /* temporary vertices */
  GLfloat bl[4]={1.0,1.0,1.0,1.0}; /* black color */
  float texCoord[4][2]={{0, 0}, {1, 0}, {1, 1}, {0, 1}};

#ifdef DEBUG
  fprintf(stderr, "createBlockDisplayList(%p)\n", block);
#endif
  
  vertices=block->form->vertices;
  polygons=block->form->polygons;

  /* begin the new list */
  block->displayListId = glGenLists(1);
  if(block->displayListId == 0) {
    fprintf(stderr, "error: Could not allocate display list\n");
    exit(1);
  }
  glNewList(block->displayListId, GL_COMPILE);

  /* Draw polygons if asked */
  if (drawSolid) {
    glEnable(GL_BLEND);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImgs[2]->sizeX, 
                 texImgs[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 (GLvoid *) texImgs[2]->data);
    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, block->material);
    /* loop through the whole BOPolygon-list */
    for (i = 0; i < block->form->numPolygons; i++) {
      p=polygons + i;

      glNormal3f(p->normal[0], p->normal[1], p->normal[2]);
      if(p->numVertices == 4)
        glBegin(GL_QUADS);
      else
        glBegin(GL_POLYGON);

      /* draw a polygon */
      for (j = 0; j < p->numVertices; j++) {
        glTexCoord2f(texCoord[j][0], texCoord[j][1]);
        glVertex3f(
          vertices[p->vertices[j]][0] - block->form->centerPoint[0] - 0.5,
          vertices[p->vertices[j]][1] - block->form->centerPoint[1] - 0.5,
          vertices[p->vertices[j]][2] - block->form->centerPoint[2] - 0.5);
      }
      glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  }

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glColor4f(bl[0], bl[1], bl[2], bl[3]);
  /* Draw wires */
  for(i = 0; i < block->form->numPolylines; i++) {
    BOPolyline *pl = block->form->polylines + i;
    int isLoop, j;

    /* Check if polyline is a loop */
    isLoop = 1;
    for(j = 0; j < 3; j++)
      if(fabs(vertices[pl->vertices[0]][j] -
              vertices[pl->vertices[pl->numVertices - 1]][j]) > 0.5) {
        isLoop = 0;
        break;
      }

    /* Draw polyline */
    if(isLoop)
      glBegin(GL_LINE_LOOP);
    else
      glBegin(GL_LINE_STRIP);
    for(j = isLoop; j < pl->numVertices; j++)
      glVertex3f(
        vertices[pl->vertices[j]][0] - block->form->centerPoint[0] - 0.5,
        vertices[pl->vertices[j]][1] - block->form->centerPoint[1] - 0.5,
        vertices[pl->vertices[j]][2] - block->form->centerPoint[2] - 0.5);
    glEnd();
  }
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  
  /* end of the list */
  glEndList();
}

void deleteBlockDisplayList(Block *block)
{
#ifdef DEBUG
  fprintf(stderr, "deleteBlockDisplayList(%p)\n", block);
#endif
  
  glDeleteLists(block->displayListId, 1);
}

Block *createBlock(Form *form, int drawSolid)
{
  Block *block;
  int i, j;

#ifdef DEBUG
  fprintf(stderr, "createBlock(%p, %d)\n", form, drawSolid);
#endif
  
  /* Allocate space for block */
  if((block = malloc(sizeof(Block))) == NULL) {
    fprintf(stderr, "Error: Not enough memory\n");
    exit(1);
  }

  /* initiate all the needed values */
  block->form = form;
  for(i=0; i<2; i++)
    block->isMoving[i] = 0;
  block->targetPosition[0] = block->position[0]=block->form->centerPoint[0];
  block->targetPosition[1] = block->position[1]=block->form->centerPoint[1];
  block->position[2]=0;
  for(i=0; i<3; i++)
    for(j=0; j<3; j++) {
      block->lastOrientation[i][j] = (i==j ? 1 : 0);
      block->orientation[i][j] = (i==j ? 1 : 0);
      block->targetOrientation[i][j] = (i==j ? 1 : 0);
    }
  block->maxTurns = 0;
  block->numTurns = 0;
  block->turns = NULL;
  block->lastFall = 0;
  block->lastStop = 0;
  block->stopHeight = -5;
  block->material[0]=0.0;
  block->material[1]=0.0;
  block->material[2]=1.0;
  block->material[3]=0.7;
  block->fallSpeed=0.1;
  block->moveSpeed=2;
  block->turnSpeed=3;

  /* Initialize display list */
  createBlockDisplayList(block, drawSolid);

  /* Initialize timer for the block */
  resetTimer(&(block->timer));
  startTimer(&(block->timer));

  return block;
}

/* Free all resources associated with a block */
void deleteBlock(Block *block)
{
#ifdef DEBUG
  fprintf(stderr, "deleteBlock(%p)\n", block);
#endif
  
  deleteBlockDisplayList(block);
  if(block->turns != NULL)
    free(block->turns);
  free(block);
}

/* Function that updates the position and orientation of the block */
void updateBlock(Block *block, float nowTime)
{

  int i, j;

#ifdef DEBUG
  fprintf(stderr, "updateBlock(%p, %4.2g)\n", block, nowTime);
#endif

  /* Update block position */
  for(i=0; i<2; i++)
    if(block->isMoving[i]) {
      float move;
      
      /* Calculate new position */
      move = (nowTime - block->lastMove[i]) * block->moveSpeed;
      if(move >= fabs(block->lastPosition[i] - block->targetPosition[i])) {
        /* Movement finished */
        block->lastPosition[i] = block->position[i] = block->targetPosition[i];
        block->isMoving[i] = 0;
      } else {
        /* Update position */
        if(block->targetPosition[i] < block->lastPosition[i])
          block->position[i] = block->lastPosition[i] - move;
        else
          block->position[i] = block->lastPosition[i] + move;
      }
    }

  /* Remove turns that are finished */
  for(i=0; i<block->numTurns; i++) {
    TurnInfo *turn;
    
    /* Check if the turn is still unfinished */
    turn = block->turns + i;
    if(nowTime - turn->startTime < 1 / block->turnSpeed)
      break;

    /* Update last orientation */
    rotateIntegerRotation(block->lastOrientation, turn->axis, turn->sign);
  }
  if(i > 0) {
    block->numTurns -= i;
    memmove(block->turns,
            block->turns + i,
            block->numTurns * sizeof(TurnInfo));
  }

  /* Update orientation */
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      block->orientation[i][j] = block->lastOrientation[i][j];
  for(i=0; i<block->numTurns; i++) {
    float rot[3][3], tmp[3][3];
    float a[3] = {0, 0, 0};
    TurnInfo *turn;

    turn = block->turns + i;
    a[turn->axis] = ((nowTime - turn->startTime) * block->turnSpeed)
      * turn->sign * PI/2;
    createRotation(a, rot);
    rotateRotation(block->orientation, rot, tmp);
    memcpy(block->orientation, tmp, sizeof(int [3][3]));
  }
}

/* Move block to given position. Assumes that updateBlock() has already been
   called for the same time stamp. */
void moveBlock(Block *block, int axis, int sign, float nowTime)
{
#ifdef DEBUG
  fprintf(stderr, "moveBlock(%p, %d, %d, %4.2g)\n",
          block, axis, sign, nowTime);
#endif
  assert(axis >= 0 && axis < 3);
  assert(sign == -1 || sign == 1);

  block->lastPosition[axis] = block->position[axis];
  block->lastMove[axis] = nowTime;
  block->targetPosition[axis] += sign;
  block->isMoving[axis] = 1;
}
 
/* Turn block to given orientation. */
void turnBlock(Block *block, int axis, int sign, float nowTime)
{
  TurnInfo *turn; /* pointer to new structure */

#ifdef DEBUG
  fprintf(stderr, "turnBlock(%p, %d, %d, %4.2g)\n",
          block, axis, sign, nowTime);
#endif
  assert(axis >= 0 && axis < 3);
  assert(sign == -1 || sign == 1);

  /* Allocate space for turn info if necessary */
  if(block->maxTurns == 0) {
    block->turns = malloc(sizeof(TurnInfo) * 4);
    assert(block->turns != NULL);
    block->maxTurns = 4;
  } else if(block->numTurns >= block->maxTurns) {
    block->maxTurns *= 2;
    block->turns = realloc(block->turns, sizeof(TurnInfo) * block->maxTurns);
    assert(block->turns != NULL);
  }

  /* Initialize turn info */
  turn = block->turns + block->numTurns;
  turn->startTime = nowTime;
  turn->axis = axis;
  turn->sign = sign;

  /* Update target orientation */
  rotateIntegerRotation(block->targetOrientation, axis, sign);

  /* Increase turn count and continue */
  block->numTurns++;
}

void drawBlock(Block *block)
{
#ifdef DEBUG
  fprintf(stderr, "drawBlock(%p)\n", block);
#endif
  
  glCallList(block->displayListId);
}

void getBlockDimensions(Block *block, int or[3][3], int dim[3])
{
  int i;

#ifdef DEBUG
  fprintf(stderr, "getBlockDimensions(%p, %p, %p)\n",
          block, or, dim);
#endif
  
  rotateIntegerVector(block->form->dimensions, or, dim);
  for(i=0; i<3; i++)
    if(dim[i] < 0)
      dim[i] = -dim[i];
}

void getBlockCenter(Block *block, int dim[3], int or[3][3], int cp[3])
{
  int i, cp2[3];

#ifdef DEBUG
  fprintf(stderr, "getBlockCenter(%p, [%d, %d, %d], %p, %p)\n",
          block, dim[0], dim[1], dim[2], or, cp);
#endif
  
  memcpy(cp2, block->form->centerPoint, sizeof(int [3]));
  for(i=0; i<3; i++)
    cp2[i]++;
  rotateIntegerVector(cp2, or, cp);
  for(i=0; i<3; i++)
    if(cp[i] < 0)
      cp[i] += dim[i];
    else
      cp[i]--;
}

int getBlockCube(Block *block, int pos[3], int or[3][3])
{
  int p[3], rpos[3];
  int i, ror[3][3];
  int rdim[3];

#ifdef DEBUG
  fprintf(stderr, "getBlockCube(%p, [%d, %d, %d], %p)\n",
          block, pos[0], pos[1], pos[2], or);
#endif
  
  getBlockDimensions(block, or, rdim);

  for(i=0; i<3; i++)
    p[i] = pos[i]+1;
  inverseIntegerRotation(or, ror);
  rotateIntegerVector(p, ror, rpos);
  for(i=0; i<3; i++)
    if(rpos[i] < 0)
      rpos[i] += block->form->dimensions[i];
    else
      rpos[i]--;

  for(i=0; i<3; i++)
    if(!(rpos[i] >= 0 && rpos[i] < block->form->dimensions[i])) {
      fprintf(stderr, "rpos = [%d, %d, %d], rdim = [%d, %d, %d], "
              "pos = [%d, %d, %d], dim = [%d, %d, %d]\n",
              rpos[0], rpos[1], rpos[2],
              rdim[0], rdim[1], rdim[2],
              pos[0], pos[1], pos[2],
              block->form->dimensions[0],
              block->form->dimensions[1],
              block->form->dimensions[2]);
      exit(1);
    }
  
  return block->form->cubes[
    rpos[2] * block->form->dimensions[0] * block->form->dimensions[1] +
    rpos[1] * block->form->dimensions[0] +
    rpos[0]];
}
