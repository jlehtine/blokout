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
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "forms.h"
#include "transforms.h"

typedef struct SearchArgs SearchArgs;

struct SearchArgs {
  Form *form;
  int longest;
  int longestIsLoop;
  int *edges;
  int *stack;
  int *path;
  int stackP;
};

int numForms = 0;
Form **forms = NULL;

static int getFormCube(Form *form, int pos[3]);
static int isVisible(Form *form, int pos[3], int direction);
static int addVertexToForm(Form *form, float x, float y, float z);
static int readIntFromFile(FILE *f, int *res);
static void removeEdgesInPath(Form *form, int *edges,
                              int *path, int pathLen);
static void searchMaxPath(SearchArgs *args);

#if 0
static void printEdges(Form *form, int *edges) {
  int pos[3], i;
  
  printf("Edges left:\n");
  for(pos[0] = 0; pos[0] <= form->dimensions[0]; pos[0]++)
    for(pos[1] = 0; pos[1] <= form->dimensions[1]; pos[1]++)
      for(pos[2] = 0; pos[2] <= form->dimensions[2]; pos[2]++)
        for(i = 0; i<3; i++)
          if(edges[
            (pos[2] * (form->dimensions[0] + 1) *
             (form->dimensions[1] + 1) +
             pos[1] * (form->dimensions[0] + 1) +
             pos[0]) * 3 + i])
            printf("  [%d, %d, %d, %d]\n",
                   pos[0], pos[1], pos[2], i);
}
#endif

/* Reads an integer from file. Returns 0 on end-of-file */
static int readIntFromFile(FILE *f, int *res)
{
  int c;
  
  /* Skip whitespace */
  c = getc(f);
  while(!isdigit(c) && c != '-') {
    if(c == '#')
      while((c = getc(f)) != '\n' && c != EOF);
    else if((c = getc(f)) == EOF)
      return 0;
  }
  ungetc(c, f);
  return fscanf(f, "%d", res);
}

/* Read forms from given file. Returns 1 on success and 0 on failure. */
int readForms(char *fname)
{
  FILE *f;
  int i;

  /* Open the file */
  if((f = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Error, could not open form data file \"%s\"\n", fname);
    return 0;
  }
  
  /* Read number of forms */
  if(readIntFromFile(f, &numForms) != 1) {
    fprintf(stderr, "Error, invalid format reading form data\n");
    return 0;
  }

  /* Allocate space for forms */
  if((forms = malloc(sizeof(Form *) * numForms)) == NULL) {
    fprintf(stderr, "Error, not enough memory while reading forms\n");
    exit(1);
  }

  /* Read form types */
  for(i=0; i< numForms; i++) {
    int j;
    int dim[3];
    
    /* Read form dimensions */
    for(j=0; j<3; j++)
      if(readIntFromFile(f, dim+j) != 1) {
        fprintf(stderr, "Error, invalid format reading form data\n");
        return 0;
      }

    /* Create form */
    forms[i] = createForm(dim[0], dim[1], dim[2]);

    /* Read cube values */
    for(dim[2]=0; dim[2] < forms[i]->dimensions[2]; dim[2]++)
      for(dim[1]=0; dim[1] < forms[i]->dimensions[1]; dim[1]++)
        for(dim[0]=0; dim[0] < forms[i]->dimensions[0]; dim[0]++) {
          int j;
          if(readIntFromFile(f, &j) != 1) {
            fprintf(stderr, "Error, invalid form reading form data\n");
            return 0;
          }
          if (j != 0) {
            forms[i]->cubes[dim[2] * forms[i]->dimensions[1] *
                           forms[i]->dimensions[0] + dim[1] *
                           forms[i]->dimensions[0] +
                           dim[0]] = 1;
            forms[i]->numCubes++;
          }
        }
    /* Read centerpoint */
    for(j=0; j<3; j++)
      if(readIntFromFile(f, forms[i]->centerPoint+j) != 1) {
        fprintf(stderr, "Error, invalid format reading form data\n");
        return 0;
      }
  }

  /* Close file */
  fclose(f);
  return 1;
}

/* function than initializes a new form of given size */
Form *createForm(int x, int y, int z)
{
  Form *form; /* new form */

#ifdef DEBUG
  fprintf(stderr, "createForm(%d, %d, %d)\n", x, y, z);
#endif
  
  form = malloc(sizeof(Form));
  if (form == NULL) {
    fprintf(stderr,"Error, not enough memory while creating a form\n");
    exit(1);
  }
  
  /* initialize the dimensions */
  form->dimensions[0]=x;
  form->dimensions[1]=y;
  form->dimensions[2]=z;

  /* initialize centerpoint */
  form->centerPoint[0] = form->centerPoint[1] = form->centerPoint[2] = 0;
  
  form->cubes=malloc(sizeof(int)*x*y*z);
  if (form->cubes == NULL) {
    fprintf(stderr,"Error, not enough memory while creating a form\n");
    exit(1);
  }
  form->numCubes=0;

  /* initialize the cubes to zero */
  memset(form->cubes, 0, sizeof(int) * x * y * z);
  form->numVertices=0;
  form->vertices=NULL;
  form->numPolygons=0;
  form->polygons=NULL;
  form->numPolylines=0;
  form->polylines=NULL;

  return form;
}

void deleteForm(Form *form)
{
  int i; /* index */

#ifdef DEBUG
  fprintf(stderr, "deleteForm(%p)\n", form);
#endif
  
  for (i=0; i < form->numPolygons; i++)
    free(form->polygons[i].vertices);
  free(form->vertices);
  free(form->cubes);
}

void addCubeToForm(Form *form, int x,int y,int z)
{
#ifdef DEBUG
  fprintf(stderr, "addCubeToForm(%p, %d, %d, %d)\n",
          form, x, y, z);
#endif
  
  form->cubes[(z * form->dimensions[0] * form->dimensions[1] +
	       y * form->dimensions[1] + x)] = 1;
}

static int getFormCube(Form *form, int pos[3])
{
#ifdef DEBUG
  fprintf(stderr, "getFormCube(%p, [%d, %d, %d])\n",
          form, pos[0], pos[1], pos[2]);
#endif
  
  return form->cubes[pos[2] * form->dimensions[0] * form->dimensions[1] +
                    pos[1] * form->dimensions[0] +
                    pos[0]];
}

/* a little function to help us decide whether we need to draw the facet */

static int isVisible(Form *form, int pos[3], int direction)
{
  int axel, s;
  int adjacentPos[3];

#ifdef DEBUG
  fprintf(stderr, "isVisible(%p, [%d, %d, %d], %d)\n",
          form, pos[0], pos[1], pos[2], direction);
#endif
  
  /* Check if there is nothing in the cube */
  if(! getFormCube(form, pos))
    return 0;
  
  /* Check if this is outer wall */
  axel = direction >> 1;
  s = (direction & 1)*2 - 1;
  if((s < 0 && pos[axel] == 0) ||
     (s > 0 && pos[axel] == form->dimensions[axel] - 1))
    return 1;
  
  /* Check the adjacent position */
  adjacentPos[0] = pos[0];
  adjacentPos[1] = pos[1];
  adjacentPos[2] = pos[2];
  adjacentPos[axel] += s;
  return getFormCube(form, adjacentPos) ? 0 : 1;
}

/* function to add vertex to form. Returns position of vertex in form */

static int addVertexToForm(Form *form, float x, float y, float z)
{
  int i; /* index */
  Vertex *newVertex;

#ifdef DEBUG
  fprintf(stderr, "addVertexToForm(%p, %4.2g, %4.2g, %4.2g)\n",
          form, x, y, z);
#endif
  
  /* check if vertex exists already in the global array */
  for (i=0; i < form->numVertices; i++) {
    if (fabs(form->vertices[i][0] - x) < 0.5 &&
	fabs(form->vertices[i][1] - y) < 0.5 &&
	fabs(form->vertices[i][2] - z) < 0.5)
      return i; /* return the position of vertex */
  }

  /* we have to allocate memory for a new vertex. */
  newVertex=realloc(form->vertices,sizeof(Vertex) *
                    (form->numVertices+1));

  /* assign the co-ordinate values to new vertex */
  newVertex[i][0] = x;
  newVertex[i][1] = y;
  newVertex[i][2] = z;

  form->vertices=newVertex; /* update also the vertice-list in form */
  form->numVertices++;

  return i;  /* return the position of vertex */
}


void clearFormPolygonList(Form *form)
{
  if(form->vertices != NULL)
    free(form->vertices);
  if(form->polygons != NULL)
    free(form->polygons);
  form->numVertices=0;
  form->vertices=NULL;
  form->numPolygons=0;
  form->polygons=NULL;
}

/* Create a list of polygons for form */
void createFormPolygonList(Form *form)
{
  int pos[3]; /* position */
  int dir; /* direction */
  Polygon *newPolygon; /* list of polygons */
  float vertices[4][3] = {
    {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}};

#ifdef DEBUG
  fprintf(stderr, "createFormPolygonList(%p)\n", form);
#endif
  
  /* add all Vertices to array and create polygons */ 
  for (pos[2]=0; pos[2] < form->dimensions[2]; pos[2]++)
    for (pos[1]=0; pos[1] < form->dimensions[1]; pos[1]++)
      for (pos[0]=0; pos[0] < form->dimensions[0]; pos[0]++)
        for(dir=0; dir<6; dir++)
          if(isVisible(form, pos, dir)) {
            int i, j;
            float m[3][3];
            float v[3];
            Polygon *p;
            
            i=form->numPolygons;
            /* first reserve enough memory for new Polygon ... */
            newPolygon=realloc(form->polygons,sizeof(Polygon) * (i+1));
            if (newPolygon == NULL) {
              fprintf(stderr,
                      "Error, not enough memory for storing polygons.\n");
              exit(1);
            }
            /* ... and for all the vertices in Polygon ... */
            newPolygon[i].numVertices=4;
            newPolygon[i].vertices=malloc(sizeof(int)*4);
            if (newPolygon->vertices == NULL) {
              fprintf(stderr,
                      "Error, not enough memory for storing polygons.\n");
              exit(1);
            }
            
            /* update the polygon-list and the number of polygons */
            form->polygons=newPolygon;
            form->numPolygons++;

            /* Add the vertices */
            createRotationForDirection(dir, m);
            for(j=0; j<4; j++) {
              rotateVector(vertices[j], m, v);
              newPolygon[i].vertices[j] =
                addVertexToForm(form,
                                v[0]+0.5+pos[0],
                                v[1]+0.5+pos[1],
                                v[2]+0.5+pos[2]);
            }

            /* Calculate the normal */
            p = newPolygon + i;
            p->normal[0] = p->normal[1] = p->normal[2] = 0;
            p->normal[dir >> 1] = (dir & 1)*2 - 1;
          }
}

static void removeEdgesInPath(Form *form, int *edges,
                              int *path, int pathLen)
{
  int i;
  int pos[3];

#ifdef DEBUG
  fprintf(stderr, "removeEdgesInPath(%p, %p, %p, %d)\n", form, edges, path,
          pathLen);
#endif
  
  /* Read the starting position */
  for(i = 0; i < 3; i++)
    pos[i] = path[i];

  /* Follow the path */
  for(i = 1; i <= pathLen; i++) {
    int axis = 0, dir = 0;
    int j;

    /* Check movement */
    for(j = 0; j < 3; j++)
      if(path[i*3 + j] != pos[j]) {
        axis = j;
        dir = path[i*3 + j] - pos[j];
        break;
      }

    /* Delete edge */
    if(dir < 0)
      pos[axis] += dir;
    edges[(pos[2] * (form->dimensions[0] + 1) * (form->dimensions[1] + 1) +
           pos[1] * (form->dimensions[0] + 1) +
           pos[0]) * 3 + axis] = 0;
    if(dir > 0)
      pos[axis] += dir;
  }
}

static void searchMaxPath(SearchArgs *args)
{
  int axis, dir;
  int npos[3], tpos[3];
  int i, notFound;
  int *edgeFlag;

#ifdef DEBUG
  fprintf(stderr, "searchMaxPath(%p)\n", args);
#endif
  
  /* Read current position */
  for(i=0; i<3; i++)
    npos[i] = args->stack[3 * args->stackP + i];

  /* Check if we would break a line loop */
  for(i=0; i < 3; i++)
    if(args->stack[i] != npos[i])
      break;

  notFound = 1;
  if(args->stackP == 0 || i < 3) {
    memcpy(tpos, npos, sizeof(int[3]));

    /* If we can continue to same direction, then continue to same direction */
    if(args->stackP > 0) {
      for(axis = 0; axis < 3; axis++)
        if((dir = args->stack[args->stackP * 3 + axis] -
            args->stack[(args->stackP-1) * 3 + axis]) != 0)
          break;
      if((npos[axis] > 0 || dir > 0) &&
         (npos[axis] < args->form->dimensions[axis] || dir < 0)) {
        if(dir < 0)
          tpos[axis]--;
        edgeFlag = args->edges +
          (tpos[2] * (args->form->dimensions[0]+1) *
           (args->form->dimensions[1]+1) +
           tpos[1] * (args->form->dimensions[0]+1) +
           tpos[0]) * 3 + axis;
        if(*edgeFlag) {
          npos[axis] += dir;
          
          /* Try this route */
          notFound = 0;
          args->stackP++;
          for(i = 0; i < 3; i++)
            args->stack[3 * args->stackP + i] = npos[i];
          *edgeFlag = 0;
          searchMaxPath(args);
          *edgeFlag = 1;
          args->stackP--;
        }
        if(dir < 0)
          tpos[axis]++;
      }
    }
    
    /* Otherwise, try all possibilities */
    if(notFound) {
      for(axis = 0; axis < 3; axis++)
        for(dir = 0; dir < 2; dir++) {
          tpos[axis] += (dir-1);
          edgeFlag = args->edges +
            (tpos[2] * (args->form->dimensions[0]+1) *
             (args->form->dimensions[1]+1) +
             tpos[1] * (args->form->dimensions[0]+1) +
             tpos[0]) * 3 + axis;
          if(tpos[axis] < args->form->dimensions[axis] && tpos[axis] >= 0 &&
             *edgeFlag) {
            npos[axis] += (dir*2)-1;
            
            /* Try this route */
            notFound = 0;
            args->stackP++;
            for(i = 0; i < 3; i++)
              args->stack[3 * args->stackP + i] = npos[i];
            *edgeFlag = 0;
            searchMaxPath(args);
            *edgeFlag = 1;
            args->stackP--;
            
            npos[axis] -= (dir*2)-1;
          }
          tpos[axis] -= (dir-1);
        }
    }
  }

  /* Check whether new path is a loop or not */
  for(i=0; i<3; i++)
    if(args->stack[i] != args->stack[args->stackP * 3 + i])
      break;
  
  /* If no more edges were found, save current path if longest or loop */
  if(notFound &&
     (args->longest == 0 ||
      (args->stackP > 0 && !(args->longestIsLoop) && i == 3) ||
      (args->longestIsLoop && i == 3 && args->stackP > args->longest))) {
    args->longest = args->stackP;
    args->longestIsLoop = (i == 3);
    memcpy(args->path, args->stack, sizeof(int) * (args->longest+1) * 3);
  }
}

void createFormPolylineList(Form *form)
{
  int *edges, *path, *stack;
  int vpos[3];
  int i, found;

#ifdef DEBUG
  fprintf(stderr, "createFormPolylineList(%p)\n", form);
#endif

#if 0
  printf("block info:\n");
  printf("  %d x %d x %d\n",
         form->dimensions[0],
         form->dimensions[1],
         form->dimensions[2]);
#endif
  
  /* Allocate space for edges */
  if((edges = malloc(sizeof(int) * (form->dimensions[0]+1) *
                     (form->dimensions[1]+1) *
                     (form->dimensions[2]+1) * 3)) == NULL) {
    fprintf(stderr, "Error, not enough memory\n");
    exit(1);
  }

  /* Clear edges */
  memset(edges, 0, sizeof(int) *
         ((form->dimensions[0]+1) * (form->dimensions[1]+1) *
          (form->dimensions[2]+1)) * 3);

  /* Find out visible edges */
  for(vpos[0] = 0; vpos[0] <= form->dimensions[0]; vpos[0]++)
    for(vpos[1] = 0; vpos[1] <= form->dimensions[1]; vpos[1]++)
      for(vpos[2] = 0; vpos[2] <= form->dimensions[2]; vpos[2]++) {
        int i;
        
        for(i=0; i<3; i++)
          if(vpos[i] < form->dimensions[i]) {
            int cpos[3];
            int ai[3];
            int j, k;

            k = 0;
            for(j=0; j<3; j++) {
              cpos[j] = vpos[j];
              if(j == i)
                ai[2] = j;
              else
                ai[k++] = j;
            }
            for(j=0; j<2; j++)
              for(k=0; k<2; k++) {
                cpos[ai[0]] += j-1;
                cpos[ai[1]] += k-1;
                if(cpos[ai[0]] >= 0 &&
                   cpos[ai[0]] < form->dimensions[ai[0]] &&
                   cpos[ai[1]] >= 0 &&
                   cpos[ai[1]] < form->dimensions[ai[1]]) {
                  if(isVisible(form, cpos, (ai[0] << 1) | (1-j)) &&
                     isVisible(form, cpos, (ai[1] << 1) | (1-k))) {
                    edges[
                      (vpos[2] * (form->dimensions[0] + 1) *
                       (form->dimensions[1] + 1) +
                       vpos[1] * (form->dimensions[0] + 1) +
                       vpos[0]) * 3 + i] = 1;
                    j = 2;
                    break;
                  }
                  cpos[ai[0]] += (1-j)*2 - 1;
                  if(cpos[ai[0]] >= 0 &&
                     cpos[ai[0]] < form->dimensions[ai[0]]) {
                    if(isVisible(form, cpos, (ai[0] << 1) | j)) {
                      cpos[ai[1]] += (1-k)*2 - 1;
                      if(cpos[ai[1]] >= 0 &&
                         cpos[ai[1]] < form->dimensions[ai[1]]) {
                        cpos[ai[0]] -= (1-j)*2 - 1;
                        if(isVisible(form, cpos, (ai[1] << 1) | k)) {
                          edges[
                            (vpos[2] * (form->dimensions[0] + 1) *
                             (form->dimensions[1] + 1) +
                             vpos[1] * (form->dimensions[0] + 1) +
                             vpos[0]) * 3 + i] = 1;
                          j = 2;
                          break;
                        }
                        cpos[ai[0]] += (1-j)*2 - 1;
                      }
                      cpos[ai[1]] -= (1-k)*2 - 1;
                    }
                  }
                  cpos[ai[0]] -= (1-j)*2 - 1;
                }
                cpos[ai[0]] -= j-1;
                cpos[ai[1]] -= k-1;
              }
          }
      }

  /* Allocate space for path */
  i = (((form->dimensions[0] + 1) * (form->dimensions[1] + 1) *
        (form->dimensions[2] + 1)) * 3 + 1) * 3;
  if((path = malloc(sizeof(int) * i)) == NULL) {
    fprintf(stderr, "Error, not enough memory\n");
    exit(1);
  }

  /* Allocate space for stack */
  if((stack = malloc(sizeof(int) * i)) == NULL) {
    fprintf(stderr, "Error, not enough memory\n");
    exit(1);
  }
    
  /* Find out maximum paths in edge graph */
  found = 1;
  while(found) {
    int pos[3], i;
    SearchArgs args;
    int savedLength;
  
    /* Find suitable starting place */
    found = 0;
    for(pos[0] = 0; pos[0] <= form->dimensions[0]; pos[0]++)
      for(pos[1] = 0; pos[1] <= form->dimensions[1]; pos[1]++)
        for(pos[2] = 0; pos[2] <= form->dimensions[2]; pos[2]++)
          for(i = 0; i<3; i++)
            if(edges[
              (pos[2] * (form->dimensions[0] + 1) *
               (form->dimensions[1] + 1) +
               pos[1] * (form->dimensions[0] + 1) +
               pos[0]) * 3 + i]) {
              Polyline *polyline;
              
              /* Search one path */
              found = 1;
              args.form = form;
              args.longest = 0;
              args.edges = edges;
              args.stack = stack;
              args.stack[0] = pos[0];
              args.stack[1] = pos[1];
              args.stack[2] = pos[2];
              args.path = path;
              args.stackP = 0;
              searchMaxPath(&args);
              savedLength = args.longest;

              /* Remove edges used in a path */
              removeEdgesInPath(form, edges, path, savedLength);

              /* If we didn't find a loop, try making another search */
              if(path[0] != path[savedLength * 3] ||
                 path[1] != path[savedLength * 3 + 1] ||
                 path[2] != path[savedLength * 3 + 2]) {
                int j;
                
                args.longest = 0;
                args.path = path + 3*(savedLength + 1);
                args.stack[0] = pos[0];
                args.stack[1] = pos[1];
                args.stack[2] = pos[2];
                args.stackP = 0;
                searchMaxPath(&args);
                
                /* Combine paths if additional path was found */
                if(args.longest > 0) {
                  /* Remove edges of the path */
                  removeEdgesInPath(form, edges, args.path, args.longest);
              
                  /* Invert existing path */
                  for(i=0, j = savedLength; i < j; i++, j--) {
                    int tmp, k;
                    
                    for(k = 0; k < 3; k++) {
                      tmp = path[i*3 + k];
                      path[i*3 + k] = path[j*3 + k];
                      path[j*3 + k] = tmp;
                    }
                  }

                  /* Delete the common path item */
                  memmove(path + 3 * savedLength,
                          path + 3 * (savedLength + 1),
                          sizeof(int) * 3 * (args.longest + 1));
                  savedLength += args.longest;
                }
              }

              /* Remove path items were we are moving to same direction */
              for(i=1; i<savedLength; i++) {
                int axis;

                /* Check if we are moving along axis (no need to create
                   a new vertex) */
                for(axis=0; axis<3; axis++)
                  if(path[(i-1)*3 + axis] != path[i*3 + axis] &&
                     path[i*3 + axis] != path[(i+1)*3 + axis])
                    break;
                if(axis < 3) {
                  memmove(path+i*3, path+(i+1)*3,
                          sizeof(int) * (savedLength-i)*3);
                  savedLength--;
                }
              }

              /* Add polyline to form */
              form->numPolylines++;
              if((form->polylines = realloc(form->polylines,
                                            sizeof(Polyline) *
                                            form->numPolylines)) == NULL) {
                fprintf(stderr, "Error, not enough memory\n");
                exit(1);
              }
              polyline = form->polylines + form->numPolylines - 1;

              /* Add vertices to polyline */
#if 0
              printf("Polyline with %d vertices:\n", savedLength+1);
#endif
              polyline->numVertices = savedLength + 1;
              if((polyline->vertices = malloc(sizeof(int) * (savedLength+1)))
                 == NULL) {
                fprintf(stderr, "Error, not enough memory\n");
                exit(1);
              }
              for(i=0; i <= savedLength; i++) {
                polyline->vertices[i] = 
                  addVertexToForm(form, path[i*3], path[i*3+1], path[i*3+2]);
#if 0
                printf("  [%d, %d, %d]\n", path[i*3], path[i*3+1],
                       path[i*3+2]);
#endif
              }
            }
  }
  free(stack);
  free(path);
  free(edges);
  return;
}
