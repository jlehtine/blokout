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

/*
  Block Out v. 0.1a
  
  Authors: Johannes Lehtinen
           Petri Salmi
  
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glut.h>
#include "game.h"
#include "timing.h"
#include "menu.h"
#include "select.h"
#include "texture.h"
#include "highscore.h"
#include "highscoreui.h"

int main(int argc, char *argv[])
{
  int i, *j;

  /* start the timer */
  startTimer(&mainTimer);
  /* Read form data */
  readForms("forms.dat");
  /* initiate blocks in game */
  j=malloc(sizeof(int) * numForms);
  assert(j != NULL);
  
  /* Create vertex lists for forms */
  for(i=0; i<numForms; i++) {
    createFormPolygonList(forms[i]);
    createFormPolylineList(forms[i]);
    j[i]=1;
  }
  initGameBlocks(numForms, j);

  /* initiate textures */
  initTextures(); 

  screenSize[0] = 640;
  screenSize[1] = 480;
  glutInit( &argc, argv );

  /* Read possible command line arguments */
  for(i=1; i<argc; i++) {
    fprintf(stderr, "Error, unknown argument \"%s\"\n", argv[i]);
    exit(1);
  }
  
  glutInitWindowSize( screenSize[0], screenSize[1] );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow(argv[0]);

  initGame();
  initGameGraphics();
  initMenu();
  menuStartTime = readTimer(&mainTimer);
  menuMode();
  
  glutMainLoop();
  return 0;
}  
