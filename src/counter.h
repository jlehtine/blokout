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

#ifndef __COUNTER_H_INCLUDED__
#define __COUNTER_H_INCLUDED__

#include <GL/gl.h>

typedef struct Counter Counter;

struct Counter {
  int value;            /* Current number in counter */
  float angle;          /* begin angle */
  float currentAngle;   /* the angle counter has already moved */
  float diff;           /* difference from normal direction */
  int isRotating;       /* is counter in move */
  GLuint displayListId; /* Display list ID */
  int numTurns;         /* number of buffered turns */
  int maxTurns;         /* maximum number of simultaneous turns */
  TurnInfo *turns;      /* buffered rotations */
  float turnSpeed;      /* speed of turning */

  /* there are only for displaying */
  float position[3];       /* Current position in picture */
  float orientation[3][3]; /* Current orientation */
};

Counter * newCounter(void);
void deleteCounter(Counter *counter);
void drawCounter(Counter *counter);
int addCounter(Counter *counter, int amount);
void updateCounter(Counter *counter);

#endif
