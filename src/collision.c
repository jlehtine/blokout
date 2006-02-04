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
#include <math.h>
#include <string.h>
#include "transforms.h"
#include "blocks.h"
#include "tube.h"
#include "collision.h"
#include "timing.h"

static int isLegalMove(Tube *tube, Block *block, int axis, int sign);
static int isLegalTurn(Tube *tube, Block *block, int axis, int sign);
static int isLegalPosition(
  Tube *tube, Block *block, int blockDim[3], int blockCP[3],
  int or[3][3], int pos[3]);

static int isLegalMove(Tube *tube, Block *block, int axis, int sign)
{
  int blockDim[3], blockCP[3];
  int tp[3];

  
#ifdef DEBUG
  fprintf(stderr, "isLegalMove(%p, %p, %d, %d)\n", tube, block, axis, sign);
#endif

  /* Check against tube dimensions */
  getBlockDimensions(block, block->targetOrientation, blockDim);
  getBlockCenter(block, blockDim, block->targetOrientation, blockCP);
  if(sign < 0 && block->targetPosition[axis] <= blockCP[axis])
    return 0;
  else if(sign > 0 &&
          block->targetPosition[axis] - blockCP[axis] + blockDim[axis] >=
          tube->form->dimensions[axis])
    return 0;

  tp[0]=block->targetPosition[0];
  tp[1]=block->targetPosition[1];
  tp[2]=floor(block->position[2]);
  tp[axis] += sign;

  /* check against other cubes */
  return isLegalPosition(tube, block, blockDim, blockCP,
                         block->targetOrientation, tp);
}

static int isLegalTurn(Tube *tube, Block *block, int axis, int sign)
{
  int blockDim[3], blockCP[3];
  int pos[3];
  
#ifdef DEBUG
  fprintf(stderr, "isLegalTurn(%p, %p, %d, %d)\n", tube, block, axis, sign);
#endif

  pos[0]=block->targetPosition[0];
  pos[1]=block->targetPosition[1];
  pos[2]=floor(block->position[2]);

  getBlockDimensions(block, block->targetOrientation, blockDim);
  getBlockCenter(block, blockDim, block->targetOrientation, blockCP);

  return isLegalPosition(tube, block, blockDim, blockCP,
                         block->targetOrientation, pos);
}

static int isLegalPosition(
  Tube *tube, Block *block, int blockDim[3], int blockCP[3],
  int or[3][3], int pos[3])
{
  int ok, p[3];

#ifdef DEBUG
  fprintf(stderr,
          "isLegalPosition(%p, %p, [%d, %d, %d], [%d, %d, %d], "
          "[%d, %d, %d])\n",
          tube, block,
          blockDim[0], blockDim[1], blockDim[2],
          blockCP[0], blockCP[1], blockCP[2],
          pos[0], pos[1], pos[2]);
#endif
  
  ok = 1;
  for(p[2]=0; ok && p[2]<blockDim[2]; p[2]++)
    for(p[1]=0; ok && p[1]<blockDim[1]; p[1]++)
      for(p[0]=0; ok && p[0]<blockDim[0]; p[0]++) {
        if(tube->form->cubes[
          (pos[2]+p[2]+tube->form->dimensions[2]-blockCP[2]) *
          tube->form->dimensions[0] *
          tube->form->dimensions[1] +
          (pos[1]+p[1]-blockCP[1]) *
          tube->form->dimensions[0] +
          (pos[0]+p[0]-blockCP[0])] &&
           getBlockCube(block, p, block->targetOrientation)) {
          ok = 0;
          break;
        }
      }
  return ok;
}

void tryMoveBlock(Tube *tube, Block *block, int axis, int sign, float time)
{
#ifdef DEBUG
  fprintf(stderr, "tryMoveBlock(%p, %p, %d, %d, %4.2g)\n",
          tube, block, axis, sign, time);
#endif
  
  if(isLegalMove(tube, block, axis, sign))
    moveBlock(block, axis, sign, time);
}

void tryTurnBlock(Tube *tube, Block *block, int axis, int sign, float time)
{
  int blockDim[3], blockCP[3];
  int or[3][3];
  int i, ok;
  int moves[3] = {0, 0, 0};

#ifdef DEBUG
  fprintf(stderr, "tryTurnBlock(%p, %p, %d, %d, %4.2g)\n",
          tube, block, axis, sign, time);
#endif
  
  /* Check if we have to move the block */
  memcpy(or, block->targetOrientation, sizeof(int [3][3]));
  rotateIntegerRotation(block->targetOrientation, axis, sign);
  getBlockDimensions(block, block->targetOrientation, blockDim);
  getBlockCenter(block, blockDim, block->targetOrientation, blockCP);
  ok = 1;
  for(i=0; i<2 && ok; i++) {
    /* Check if we have to move block to positive direction on this axis */
    while(block->targetPosition[i] < blockCP[i]) {
      if(isLegalMove(tube, block, i, 1)) {
        block->targetPosition[i]++;
        moves[i]++;
      }
      else {
        ok = 0;
        break;
      }
    }

    /* Check if we have to move block to negative direction on this axis */
    while(block->targetPosition[i] - blockCP[i] + blockDim[i] >
          tube->form->dimensions[i]) {
      if(isLegalMove(tube, block, i, -1)) {
        block->targetPosition[i]--;
        moves[i]--;
      }
      else {
        ok = 0;
        break;
      }
    }
  }

  /* Check if the turn would be legal */
  if(ok && !isLegalTurn(tube, block, axis, sign))
    ok = 0;
  memcpy(block->targetOrientation, or, sizeof(int [3][3]));

  /* Move the block if necessary */
  for(i=0; i<2; i++)
    block->targetPosition[i] -= moves[i];
  if(ok) {
    for(i=0; i<2; i++) {
      while(moves[i] < 0) {
        moveBlock(block, i, -1, time);
        moves[i]++;
      }
      while(moves[i] > 0) {
        moveBlock(block, i, 1, time);
        moves[i]--;
      }
    }
    turnBlock(block, axis, sign, time);
  }
}

/* Check if the block can be lowered to new height */
int tryLowerBlock(Tube *tube, Block *block, float time)
{
  int blockDim[3], blockCP[3];
  float newHeight;

#ifdef DEBUG
  fprintf(stderr, "tryLowerBlock(%p, %p, %4.2g)\n", tube, block, time);
#endif
  
  /* Calculate block dimensions and center point */
  getBlockDimensions(block, block->targetOrientation, blockDim);
  getBlockCenter(block, blockDim, block->targetOrientation, blockCP);

  /* Calculate new height for the block */
  newHeight = block->stopHeight -
    ((time - block->lastStop) * block->fallSpeed);

  /* Check if it's below the fast check line */
  if(newHeight < -tube->form->dimensions[2] + blockCP[2] + tube->height + 0.1)
  {
    int pos[3];
    
    /* Make a check for bottom */
    if(newHeight < -tube->form->dimensions[2] + blockCP[2])
      newHeight = -tube->form->dimensions[2] + blockCP[2] + 0.1;

    /* Check for other blocks */
    pos[0] = block->targetPosition[0];
    pos[1] = block->targetPosition[1];
    for(pos[2] = (int)floor(block->position[2]);
        pos[2] >= (int)floor(newHeight);
        pos[2]--)
      if(!isLegalPosition(tube, block, blockDim, blockCP,
                          block->targetOrientation, pos))
        break;
    if(newHeight < pos[2]+1)
      newHeight = pos[2]+1.1;
  }

  if(newHeight < block->position[2]) {
    block->lastFall = time;
    block->position[2] = newHeight;
    return 1;
  }
  if(newHeight < block->position[2])
    block->position[2] = newHeight;
  block->stopHeight = block->position[2];
  block->lastStop = time;
  return 0;
}


