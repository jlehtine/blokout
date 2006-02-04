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

#ifndef __COLLISION_H_INCLUDED__
#define __COLLISION_H_INCLUDED__

#include "blocks.h"
#include "tube.h"

void tryMoveBlock(Tube *tube, Block *block, int axis, int sign, float time);
void tryTurnBlock(Tube *tube, Block *block, int axis, int sign, float time);
int tryLowerBlock(Tube *tube, Block *block, float time);

#endif
