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

#ifndef __SELECT_H_DEFINED__
#define __SELECT_H_DEFINED__

#include "blocks.h"

typedef struct SelectSphere SelectSphere;

struct SelectSphere {
  int radius;            /* radius of the sphere */
  int blocksPerLevel[4]; /* size of each circle */ 
  Block **blocks;        /* blocks to be displayed */
  int *isSelected;       /* is current block in game */
  int numSelected;       /* how many blocks are selected */
  int curBlock;          /* number of the form we are looking at */
  int currentLookAt[2];  /* block we are looking at */
  int nextLookAt[2];     /* block we want to look at*/
  int isViewMoving;      /* is view moving */
  int isViewUpdated;     /* is view updated */
  float lookAt[3];       /* point we are looking at currently */
  float lookAngle[2];    /* current tilt and angle of view */
  float lookPos[3];      /* position of the eye */
  float turnSpeed;       /* turn speed (degrees per sec) */
  int numTurns;          /* number of buffered turns */
  int maxTurns;          /* maximum number of simultaneous turns */
  TurnInfo *turns;       /* buffered rotations */
};

void initSphere(void);
void updateSelectView(void);
void drawSelectView(void);
void turnSphere(int axis, int sign, float nowTime);

void selectKeyboard(unsigned char key, int x, int y);
void selectSpecial(int key, int x, int y);

void copySelectedBlocks(void);
void exitSelectMode(void);

void selectMode(void);

#endif





