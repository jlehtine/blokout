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

#ifndef __FORMS_H_INCLUDED__
#define __FORMS_H_INCLUDED__

typedef struct Form Form;
typedef struct Polygon Polygon;
typedef struct Polyline Polyline;
typedef float Vertex[3];

struct Polygon
{
  int numVertices;
  int *vertices;
  float normal[3];
};

struct Polyline
{
  int numVertices;
  int *vertices;
};

struct Form
{
  int dimensions[3]; /* Dimensions for the block */
  int *cubes; /* Individual cubes */
  int numCubes; /* Number of individual cubes */ 
  int numVertices; /* Number of vertices for object */
  Vertex *vertices; /* Vertices of object */
  int numPolygons; /* Number of polygons for object */
  Polygon *polygons; /* Polygons of object */
  int numPolylines; /* Number of polylines */
  Polyline *polylines; /* Polylines of the object */
  float relativeSpeed; /* Relative speed */
  int centerPoint[3]; /* Center point of the form */
};

extern int numForms;
extern Form **forms;

int readForms(char *fname);
Form *createForm(int x, int y, int z);
void deleteForm(Form *form);
void addCubeToForm(Form *form, int x, int y, int z);
void clearFormPolygonList(Form *form);
void createFormPolygonList(Form *form);
void createFormPolylineList(Form *form);

#endif
