/*************************************************************************
 * Copyright (C) 1998-2000 Johannes Lehtinen
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
#include <assert.h>
#include <GL/gl.h>
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

  /* Print information about BlokOut */
  printf(PACKAGE_NAME " v" VERSION "\n"
         "Copyright (C) 1998-2000 Johannes Lehtinen\n"
         "Copyright (C) 1998-1999 Petri Salmi\n"
         "\n"
         "This program is free software; you can redistribute it and/or "
         "modify\n"
         "it under the terms of the GNU General Public License as "
         "published by\n"
         "the Free Software Foundation; either version 2 of the License, or\n"
         "(at your option) any later version.\n"
         "\n"
         "This program is distributed in the hope that it will be useful,\n"
         "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
         "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
         "GNU General Public License for more details.\n"
         "\n"
         "You should have received a copy of the GNU General Public License\n"
         "along with this program; if not, write to the Free Software\n"
         "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA "
         "02111-1307  USA\n\n");
  
  /* start the timer */
  startTimer(&mainTimer);
  /* Read form data */
  readForms(BODATADIR "/forms.dat");
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

  screenSize[0] = 780;
  screenSize[1] = 580;
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
