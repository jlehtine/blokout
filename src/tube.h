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

#ifndef __TUBE_H_INCLUDED__
#define __TUBE_H_INCLUDED__

#include <GL/gl.h>
#include "blocks.h"

typedef struct Tube Tube;

struct Tube
{
  int height; /* height of the cube pile */
  Form *form; /* Cubes of the tube */
  int numVertices; /* number of vertices for drawing the tube */
  Vertex *vertices; /* Vertices - || - */
  int numPolygons; /* number of polygons for drawing the tube */
  Polygon *polygons; /*         - || - */
  float viewPoint[3]; /* where we are looking */
  float viewAngle[3]; /* viewing angle, tilt and roll*/
  GLfloat material[4]; /* color of the tube */
  GLfloat gridMaterial[4]; /* color of the grid of the cube */
  GLfloat *layerMaterials; /* colors for different layers */
  int solidColor; /* is background solid ? */
  int lastDrop; /* how many levels were dropped at time */
  GLuint tubeDisplayListId; /* Display list for static structures */
  GLuint tubeBackgroundDisplayListId; /* Display list for background */
  GLuint cubeDisplayListId; /* Display list for cubes */
};

Tube *createTube(int x, int y, int z);
void createTubeDisplayList(Tube *tube);
void deleteTube(Tube *tube);
void createTubeDisplayList(Tube *tube); 
void drawTube(Tube *tube);
void drawTubeWalls(Tube *tube); 
void drawTubeGrid(Tube *tube);
void createCubesDisplayList(Tube *tube);
void drawCubes(Tube *tube); /* todo */
void addBlockToTube(Tube *tube, Block *block);
void removeTubeLayer(Tube *tube, int layer);

#endif
