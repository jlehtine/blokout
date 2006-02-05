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

#ifndef __TRANSFORMS_H_INCLUDED__
#define __TRANSFORMS_H_INCLUDED__

#undef PI
#define PI 3.14159265359

void createRotation(float angle[3], float rotation[3][3]);
void createRotationForDirection(int dir, float rotation[3][3]);
void rotateVector(float v[3], float rot[3][3], float res[3]);
void rotateRotation(float m[3][3], float rot[3][3], float res[3][3]);
void inverseRotation(float m[3][3], float res[3][3]);
void rotateIntegerVector(int v[3], int rot[3][3], int res[3]);
void rotateIntegerRotation(int m[3][3], int axis, int sign);
void inverseIntegerRotation(int m[3][3], int res[3][3]);

#endif
