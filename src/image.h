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

#ifndef __IMAGE_H_INCLUDED__
#define __IMAGE_H_INCLUDED__

#include <GL/gl.h>

typedef struct Image Image;

struct Image
{
  unsigned short imagic;
  unsigned short type;
  unsigned short dim;
  unsigned short sizeX, sizeY, sizeZ;
  char name[128];
  unsigned char *data;
};

Image *loadImage(char *);
void deleteImage(Image *img);

#endif
