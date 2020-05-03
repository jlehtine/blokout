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

#include <GL/gl.h>
#if HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#if HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif
#include "overlay.h"

/* Prepare for 2D graphics */
void begin2DOverlay(GLfloat screenWidth, GLfloat screenHeight)
{
  /* Disable lighting, depth testing and backface culling */
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  /* Use orthogonal projection */
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
  glMatrixMode(GL_MODELVIEW);
}

/* End 2D graphics */
void end2DOverlay(void)
{
  /* Restore original projection */
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  /* Enable lighting, depth testing and backface culling */
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

/* Draw 2D rectangle */
void drawRectangle(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top)
{
  /* Draw rectangle as a quad */
  glBegin(GL_QUADS);
  glVertex2f(left, bottom);
  glVertex2f(right, bottom);
  glVertex2f(right, top);
  glVertex2f(left, top);
  glEnd();
}

/* Get width of text */
int getTextWidth(const void *font, const char *str)
{
  int width, i;
  
  for(i=0, width=0; str[i] != '\0'; i++)
    width += glutBitmapWidth((void *)font, str[i]);
  return width;
}

/* Draw text */
void drawText(const void *font, GLfloat left, GLfloat bottom,
              const char *str)
{
  int i;

  glRasterPos2f(left, bottom);
  for(i=0; str[i] != '\0'; i++)
    glutBitmapCharacter((void *)font, str[i]);
}
