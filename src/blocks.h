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

#ifndef __BLOCKS_H_INCLUDED__
#define __BLOCKS_H_INCLUDED__

#include <time.h>
#include <GL/gl.h>
#include "timing.h"
#include "forms.h"

/* Sides of cubes */
#define LEFT   0
#define RIGHT  1
#define BOTTOM 2
#define TOP    3
#define BACK   4
#define FRONT  5

/* Couple of macros to keep code short */
#define getRotatedBlockDimensions(b, d) \
  getBlockDimensions((b), (d), (b)->targetOrientation)
#define getRotatedBlockCube(b, d) \
  getBlockCube((b), (d), (b)->targetOrientation)

/* Typedef short names */
typedef struct TurnInfo TurnInfo;
typedef struct Block Block;

/* Block is one of those falling things that you should control... */
struct Block
{
  /* Rotations around axis (orientation) are expressed in units of 90 deg */
  
  Form *form; /* Form of the block */
  Timer timer; /* Block timer (is reseted when the block appears) */
  int isMoving[2]; /* Is moving in X- or Y-direction */
  float lastMove[2]; /* Time of last X- and Y-move */
  float lastPosition[2]; /* Position when last move action made */
  float position[3]; /* Current position */
  int targetPosition[2]; /* Target position in integers */
  int maxTurns; /* For how many turns we have space allocated */
  int numTurns; /* Number of active turns (0 if not turning) */
  TurnInfo *turns; /* Information about turns */
  int lastOrientation[3][3]; /* Orientation before unfinished turns */
  float orientation[3][3]; /* Current orientation as a 3x3 rotation matrix */
  int targetOrientation[3][3]; /* Target orientation after all turns made */
  float lastFall, lastStop; /* Time last falled or stopped */
  float stopHeight; /* At which height was stopped */
  GLuint displayListId; /* Display list ID */
  GLfloat material[4]; /* color of the block */
  GLfloat wireMaterial[4]; /* color of the vertices of the block */
  float fallSpeed; /* speed of the block, units (cube) per sec */
  float moveSpeed; /* speed of move, units per sec */
  float turnSpeed; /* speed of rotations, units (90 degrees) per sec */
};

/* Info structure for block turn */
struct TurnInfo
{
  float startTime; /* when turn was initiated */
  float angle; /* used in block selection */
  int axis; /* axis used for the turn */
  int sign; /* sign of the turn (-1 or 1) */
};

Block *createBlock(Form *form, int drawSolid);
void deleteBlock(Block *block);
void createBlockDisplayList(Block *block, int drawSolid);
void deleteBlockDisplayList(Block *block);
void updateBlock(Block *block, float nowTime);
void moveBlock(Block *block, int axis, int sign, float nowTime);
void turnBlock(Block *block, int axis, int sign, float nowTime);
void getBlockDimensions(Block *block, int or[3][3], int dim[3]);
void getBlockCenter(Block *block, int dim[3], int or[3][3], int cp[3]);
int getBlockCube(Block *block, int pos[3], int or[3][3]);
void drawBlock(Block *block);

#endif
