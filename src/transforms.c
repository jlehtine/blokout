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
#include "transforms.h"
#include "blocks.h"

void createRotation(float a[3], float rotation[3][3])
{
  rotation[0][0] = cos(a[1]) * cos(a[2]);
  rotation[0][1] = cos(a[2]) * sin(a[0]) * sin(a[1]) + cos(a[0]) * sin(a[2]);
  rotation[0][2] = -(cos(a[0])*cos(a[2]) * sin(a[1])) + sin(a[0]) * sin(a[2]);
  rotation[1][0] = -(cos(a[1]) * sin(a[2]));
  rotation[1][1] = cos(a[0]) * cos(a[2]) - sin(a[0]) * sin(a[1]) * sin(a[2]);
  rotation[1][2] = cos(a[2]) * sin(a[0]) + cos(a[0]) * sin(a[1]) * sin(a[2]);
  rotation[2][0] = sin(a[1]);
  rotation[2][1] = -(cos(a[1]) * sin(a[0]));
  rotation[2][2] = cos(a[0]) * cos(a[1]);
}

void createRotationForDirection(int dir, float rotation[3][3])
{
  float a[3] = {0, 0, 0};
  
  switch(dir) {
  case FRONT:
    break;
  case BACK:
    a[0] = PI;
    break;
  case LEFT:
    a[1] = -PI/2;
    break;
  case RIGHT:
    a[1] = PI/2;
    break;
  case TOP:
    a[0] = -PI/2;
    break;
  case BOTTOM:
    a[0] = PI/2;
    break;
  }
  createRotation(a, rotation);
}

void rotateVector(float v[3], float rot[3][3], float res[3])
{
  int i;

  for(i=0; i<3; i++)
    res[i] = rot[0][i] * v[0] + rot[1][i] * v[1] + rot[2][i] * v[2];
}

void rotateRotation(float m[3][3], float rot[3][3], float res[3][3])
{
  int i, j, k;

  for(i=0; i<3; i++)
    for(j=0; j<3; j++) {
      res[i][j] = 0;
      for(k=0; k<3; k++)
        res[i][j] += rot[k][j] * m[i][k];
    }
}

void inverseRotation(float m[3][3], float res[3][3])
{
  int i, j;

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      res[i][j] = m[j][i];
}

void rotateIntegerVector(int v[3], int rot[3][3], int res[3])
{
  int i;

  for(i=0; i<3; i++)
    res[i] = rot[0][i] * v[0] + rot[1][i] * v[1] + rot[2][i] * v[2];
}

void rotateIntegerRotation(int m[3][3], int axis, int sign)
{
  float m1[3][3], m2[3][3], m3[3][3];
  int i, j;
  float a[3] = {0, 0, 0};

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m1[i][j] = m[i][j];
  a[axis] = sign*PI/2;
  createRotation(a, m2);
  rotateRotation(m1, m2, m3);
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      m[i][j] = (m3[i][j] < 0 ? -1 : 1) * (fabs(m3[i][j]) > 0.5 ? 1 : 0);
}

void inverseIntegerRotation(int m[3][3], int res[3][3])
{
  int i, j;

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      res[i][j] = m[j][i];
}
