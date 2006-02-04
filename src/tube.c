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
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <assert.h>
#include "tube.h"
#include "transforms.h"
#include "texture.h"

Tube *createTube(int x, int y, int z)
{
  Tube *tube;
  int i;

  /* allocate memory for the tube */
  tube=malloc(sizeof(Tube));
  if (tube == NULL) {
    fprintf(stderr,"Error, not enough memory while creating tube\n");
    exit(1);
  }
  /* initialize the form and viewingpoint of tube */
  tube->form=createForm(x,y,z+1);

  tube->height=0; /* there aren't any blocks in the tube */
  tube->lastDrop=0; /* no levels are descended */

  tube->viewPoint[0]=(float) x/2;
  tube->viewPoint[1]=(float) y/2;
  tube->viewPoint[2]=(float) -z*2/3;
  tube->viewAngle[0]=0;
  tube->viewAngle[1]=0;
  tube->viewAngle[2]= -PI/2;
  
  /* display list id numbers */
  tube->tubeDisplayListId=1;
  tube->tubeBackgroundDisplayListId=2;
  tube->cubeDisplayListId=3;

  /* color of the tube */
  tube->material[0]=0.3;
  tube->material[1]=0.7;
  tube->material[2]=0.3;
  tube->material[3]=1;
  tube->gridMaterial[0]=0.8;
  tube->gridMaterial[1]=0.8;
  tube->gridMaterial[2]=0.0;
  tube->gridMaterial[3]=1.0;

  /* select layer materials */
  tube->layerMaterials = malloc(sizeof(GLfloat)*4*z);
  assert(tube->layerMaterials != NULL);
  for(i=0; i<z; i++) {
    int j;

    do {
      for(j=0; j<3; j++)
        tube->layerMaterials[i*4+j] = (rand() & 1023) / 1023.0;
    } while(tube->layerMaterials[i*4] +
            tube->layerMaterials[i*4+1] +
            tube->layerMaterials[i*4+2] < 1.5);
    tube->layerMaterials[i*4+3] = 1.0;
  }
      
  /* we draw gradients too */
  tube->solidColor=0;

  tube->tubeDisplayListId = 0;
  tube->tubeBackgroundDisplayListId = 0;
  tube->cubeDisplayListId = 0;

  createTubeDisplayList(tube);

  return tube;
}

void deleteTube(Tube *tube)
{
  deleteForm(tube->form);
  free(tube->layerMaterials);
  free(tube);
}

void createTubeDisplayList(Tube *tube)
{
  static GLfloat bl[4]={0.5,0.5,0.5,1};
  static float vertices[4][3] = {{-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5},
                                 {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}};
  static float normal[3] = {0, -1, 0}, a[3] = {0, 0, 0};
  float x, y, z;
  float v[3], m[3][3];
  int i, j;

  /* Create a display list for tube background */
  if((tube->tubeBackgroundDisplayListId = glGenLists(1)) == 0) {
    fprintf(stderr, "error: Could not allocate display lists\n");
    exit(1);
  }
  glNewList(tube->tubeBackgroundDisplayListId, GL_COMPILE);

  /* Draw textured quads */
  glShadeModel(GL_SMOOTH);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texImgs[1]->sizeX, 
               texImgs[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
               (GLvoid *) texImgs[1]->data);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);

  /* Draw sides */
  for(i=0; i<4; i++) {
    /* Create rotation matrix */
    a[2] = i*PI/2.0;
    createRotation(a, m);
    
    /* Set normal */
    rotateVector(normal, m, v);
    glNormal3f(v[0], v[1], v[2]);

    /* Add vertices */
    for(j=0; j<4; j++) {
      /* Update materials */
      if(j==1 && !tube->solidColor)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bl);
      if(j==3 && !tube->solidColor)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tube->material);

      /* Add vertex */
      rotateVector(vertices[j], m, v);
      glTexCoord2f(vertices[j][0] > 0 ? tube->form->dimensions[0] : 0,
                   vertices[j][2] > 0 ? tube->form->dimensions[2] : 0);
      glVertex3f((v[0] > 0 ? tube->form->dimensions[0]+0.25 : -0.25),
                 (v[1] > 0 ? tube->form->dimensions[1]+0.25 : -0.25),
                 (v[2] < 0 ? -tube->form->dimensions[2]-0.25 : 0));
    }
  }

  /* Draw bottom */
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bl);
  glNormal3f(0, 0, -1);
  for(i=0; i<4; i++) {
    glTexCoord2f(vertices[i][0] > 0 ? tube->form->dimensions[0] : 0,
                 vertices[i][2] > 0 ? tube->form->dimensions[1] : 0);
    glVertex3f(vertices[i][0] > 0 ? tube->form->dimensions[0]+0.25 : -0.25,
               vertices[i][2] > 0 ? tube->form->dimensions[1]+0.25 : -0.25,
               -tube->form->dimensions[2]-0.25);
  }
  
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glShadeModel(GL_FLAT);

  /* Finish list */
  glEndList();

/* set the values of the dimensions */
  x= (float) tube->form->dimensions[0];
  y= (float) tube->form->dimensions[1];
  z=-(float) tube->form->dimensions[2];  

  /* list for drawing sidelines */
  if((tube->tubeDisplayListId = glGenLists(1)) == 0) {
    fprintf(stderr, "error: Could not allocate display lists\n");
    exit(1);
  }
  glNewList(tube->tubeDisplayListId,GL_COMPILE);

  glDisable(GL_LIGHTING);
  glColor4f(tube->gridMaterial[0], tube->gridMaterial[1],
            tube->gridMaterial[2], tube->gridMaterial[3]);
  
  /* we draw the bottom grid and sidelines, first horisontal... */
  
  for( x = 0; x <= (float) tube->form->dimensions[0]; x++){
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0+x,0.0,0.0);
    glVertex3f(0.0+x,0.0,z);
    glVertex3f(0.0+x,y,z);
    glVertex3f(0.0+x,y,0.0);
    glEnd();
  }

  x=(float) tube->form->dimensions[0];

  /* ... and  then vertical lines */
  for( y = 0; y <= (float) tube->form->dimensions[1]; y++){
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0,0.0+y,0.0);
    glVertex3f(0.0,0.0+y,z);
    glVertex3f(x,0.0+y,z);
    glVertex3f(x,0.0+y,0.0);
    glEnd();
  }

  y= (float) tube->form->dimensions[1];

  /* then we draw all the squares around them */
  
  for( ; z <= 0.0 ; z++){
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0,y,z);
    glVertex3f(0.0,0.0,z);
    glVertex3f(x,0.0,z);
    glVertex3f(x,y,z);
    glEnd();
  }

  glEnable(GL_LIGHTING);
  glEndList();
}

void createCubesDisplayList(Tube *tube)
{
    int i,j; /* indexes */
  Polygon *p, *polygons; /* temporary polygons */
  Vertex *vertices; /* temporary vertices */

  vertices=tube->form->vertices;
  polygons=tube->form->polygons;

  /* begin the new list */
  if(tube->cubeDisplayListId == 0)
    if((tube->cubeDisplayListId = glGenLists(1)) == 0) {
      fprintf(stderr, "error: Could not allocate displat lists\n");
      exit(1);
    }
  glNewList(tube->cubeDisplayListId, GL_COMPILE);

  glPushMatrix();
  glDisable(GL_BLEND);
  glTranslatef(0, 0, -tube->form->dimensions[2]);
  
  /* loop through the whole Polygon-list */
  for (i = 0; i < tube->form->numPolygons; i++) {

    p=polygons + i;

    /* Check if we can draw this polygon using flat shading */
    for(j=1; j<p->numVertices; j++)
      if(fabs(vertices[p->vertices[0]][2] - vertices[p->vertices[j]][2]) > 0.5)
        break;
    if(j < p->numVertices)
      glShadeModel(GL_SMOOTH);
    
    /* Specify normal */
    glNormal3f(p->normal[0], p->normal[1], p->normal[2]);
    
    /* Draw polygon */
    if(p->numVertices == 4)
      glBegin(GL_QUADS);
    else
      glBegin(GL_POLYGON);
    for (j = 0; j < p->numVertices; j++) {
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                   tube->layerMaterials+
                   (int)(floor(vertices[p->vertices[j]][2] + 0.5))*4);
      glVertex3f(vertices[p->vertices[j]][0],
		 vertices[p->vertices[j]][1],
		 vertices[p->vertices[j]][2]);
    }
    glEnd();
    glShadeModel(GL_FLAT);
  }

  /* end of the list */
  glEnable(GL_BLEND);
  glPopMatrix();
  glEndList();
}

void drawTubeWalls(Tube *tube)
{
/*  glDisable(GL_DEPTH_TEST); */
  glCallList(tube->tubeBackgroundDisplayListId);
/*  glEnable(GL_DEPTH_TEST); */
}

void drawTubeGrid(Tube *tube)
{
  glCallList(tube->tubeDisplayListId);
}

void drawTube(Tube *tube)
{
  glCallList(tube->cubeDisplayListId);
}

void drawCubes(Tube *tube)
{
  glCallList(tube->cubeDisplayListId);
}

void removeTubeLayer(Tube *tube, int layer)
{
  int i, j ,k, xd, yd; /* index */
  
  /* get the dimensions */
  xd = tube->form->dimensions[0];
  yd = tube->form->dimensions[1];

  /*remove a layer */
  for (k = layer; k<tube->height; k++)
    for (j = 0; j < yd; j++)
      for (i = 0; i < xd; i++)
	tube->form->cubes[k * xd * yd + j * xd + i] =
	  tube->form->cubes[(k+1) * xd * yd + j * xd + i];

  /* and decrease the counter */
  tube->height--;
  /* and increase number of dropped layers */
  tube->lastDrop++;
}

void addBlockToTube(Tube *tube, Block *block)
{
  int blockPos[3], blockDim[3], blockCP[3];
  int pos[3];
  int flag;

  /* Calculate block information */
  getBlockDimensions(block, block->targetOrientation, blockDim);
  getBlockCenter(block, blockDim, block->targetOrientation, blockCP);
  blockPos[0] = block->targetPosition[0];
  blockPos[1] = block->targetPosition[1];
  blockPos[2] = floor(block->position[2] + 0.5) + tube->form->dimensions[2];

  /* Add missing pieces to tube form */
  for(pos[2]=0; pos[2] < blockDim[2]; pos[2]++)
    for(pos[1]=0; pos[1] < blockDim[1]; pos[1]++)
      for(pos[0]=0; pos[0] < blockDim[0]; pos[0]++)
        if(getBlockCube(block, pos, block->targetOrientation))
          tube->form->cubes[
            (pos[2]+blockPos[2]-blockCP[2]) * tube->form->dimensions[1] *
            tube->form->dimensions[0] +
            (pos[1]+blockPos[1]-blockCP[1]) * tube->form->dimensions[0] +
            (pos[0]+blockPos[0]-blockCP[0])] = 1;

  /* Recalculate tube height */
  flag=1;
  for(pos[2]=tube->height + blockDim[2]; pos[2] >= 0; pos[2]--) {
    for(pos[1]=0; flag && pos[1] < tube->form->dimensions[1]; pos[1]++)
      for(pos[0]=0; flag && pos[0] < tube->form->dimensions[0]; pos[0]++)
        if(pos[2] < tube->form->dimensions[2] &&
           tube->form->cubes[
             pos[2] * tube->form->dimensions[0] * tube->form->dimensions[1] +
             pos[1] * tube->form->dimensions[0] +
             pos[0]]) {
          flag = 0;
          break;
        }
    if(!flag)
      break;
  }
  tube->height = pos[2]+1;
  
  /* Check for removable layers */
  for(pos[2]=blockPos[2]-blockCP[2];
      pos[2] < blockPos[2]-blockCP[2]+blockDim[2] &&
        pos[2] < tube->form->dimensions[2];
      pos[2]++) {
    int flag=1;
    
    for(pos[1]=0; flag && pos[1] < tube->form->dimensions[1]; pos[1]++)
      for(pos[0]=0; flag && pos[0] < tube->form->dimensions[0]; pos[0]++)
        if(tube->form->cubes[
          pos[2] * tube->form->dimensions[1] * tube->form->dimensions[0] +
          pos[1] * tube->form->dimensions[0] +
          pos[0]] == 0) {
          flag = 0;
          break;
        }

    if(flag) {
      removeTubeLayer(tube, pos[2]);
      pos[2]--;
    }}
      
  /* Recalculate the tube display list */
  clearFormPolygonList(tube->form);
  createFormPolygonList(tube->form);
  createCubesDisplayList(tube);
}
