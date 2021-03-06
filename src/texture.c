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
#include <GL/gl.h>
#if HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#if HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif
#include "image.h"
#include "texture.h"

Image *texImgs[NUM_TEXTURES]; /* texture images */

static char *fileName[] = { BODATADIR BOPATHSEP "score.rgb",
                            BODATADIR BOPATHSEP "s128.rgb",
                            BODATADIR BOPATHSEP "block.rgb" };

void initTextures(void)
{
  int i;

  for (i = 0; i < NUM_TEXTURES; i++)
    if((texImgs[i] = loadImage(fileName[i])) == NULL) {
      fprintf(stderr,"Error reading a texture.\n");
      exit(1);
    }
}
