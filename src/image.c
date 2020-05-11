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
#include <string.h>
#include <stdlib.h>
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#include <GL/gl.h>
#if HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#if HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif
#include "image.h"
#include "io.h"

#define IMAGIC      0x01da
#define IMAGIC_SWAP 0xda01

#define SWAP_SHORT_BYTES(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define SWAP_LONG_BYTES(x) (((((x) & 0xff) << 24) | (((x) & 0xff00) << 8)) | \
((((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24)))

typedef struct ImageType ImageType;

struct ImageType
{
  uint16_t imagic;
  uint16_t type;
  uint16_t dim;
  uint16_t sizeX, sizeY, sizeZ;
  uint32_t min, max;
  uint32_t wasteBytes;
  char name[80];
  uint32_t colorMap;
  FILE *file;
  unsigned char *tmp[5];
  uint32_t rleEnd;
  uint32_t *rowStart;
  uint32_t *rowSize;
};

static ImageType *imageOpen(const char *fileName)
{
  ImageType *image;
  uint32_t *rowStart, *rowSize, ulTmp;
  int x, i;

  image = (ImageType *)malloc(sizeof(ImageType));
  if (image == NULL) 
    {
      fprintf(stderr, "Out of memory!\n");
      exit(-1);
    }
  if ((image->file = fopen(fileName, "rb")) == NULL) 
    {
      perror(fileName);
      exit(-1);
    }

  /*
   *    Read the image header
   */
  cfread(image, 12, 1, image->file, fileName);

  /*
   *    Check byte order
   */
  if (image->imagic == IMAGIC_SWAP) 
    {
      image->type = SWAP_SHORT_BYTES(image->type);
      image->dim = SWAP_SHORT_BYTES(image->dim);
      image->sizeX = SWAP_SHORT_BYTES(image->sizeX);
      image->sizeY = SWAP_SHORT_BYTES(image->sizeY);
      image->sizeZ = SWAP_SHORT_BYTES(image->sizeZ);
    }

  for ( i = 0 ; i <= image->sizeZ ; i++ )
    {
      image->tmp[i] = (unsigned char *)malloc(image->sizeX*256);
      if (image->tmp[i] == NULL ) 
        {
          fprintf(stderr, "Out of memory!\n");
          exit(-1);
        }
    }

  if ((image->type & 0xFF00) == 0x0100) /* RLE image */
    {
      x = image->sizeY * image->sizeZ * sizeof(uint32_t);
      image->rowStart = (uint32_t *)malloc(x);
      image->rowSize = (uint32_t *)malloc(x);
      if (image->rowStart == NULL || image->rowSize == NULL) 
        {
          fprintf(stderr, "Out of memory!\n");
          exit(-1);
        }
      image->rleEnd = 512 + (2 * x);
      fseek(image->file, 512, SEEK_SET);
      cfread(image->rowStart, 1, x, image->file, fileName);
      cfread(image->rowSize, 1, x, image->file, fileName);
      if (image->imagic == IMAGIC_SWAP) 
        {
          x /= sizeof(uint32_t);
          rowStart = image->rowStart;
          rowSize = image->rowSize;
          while (x--) 
            {
              ulTmp = *rowStart;
              *rowStart++ = SWAP_LONG_BYTES(ulTmp);
              ulTmp = *rowSize;
              *rowSize++ = SWAP_LONG_BYTES(ulTmp);
            }
        }
    }
  return image;
}

static void imageClose( ImageType *image)
{
  int i;

  fclose(image->file);
  for ( i = 0 ; i <= image->sizeZ ; i++ )
    free(image->tmp[i]);
  free(image);
}

static void imageGetRow( ImageType *image, unsigned char *buf, int y, int z, const char *fileName)
{
  unsigned char *iPtr, *oPtr, pixel;
  int count;
  size_t fres;

  if ((image->type & 0xFF00) == 0x0100)  /* RLE image */
    {
      fseek(image->file, image->rowStart[y+z*image->sizeY], SEEK_SET);
      cfread(image->tmp[0], 1, image->rowSize[y+z*image->sizeY], image->file, fileName);

      iPtr = image->tmp[0];
      oPtr = buf;
      while (1) 
        {
          pixel = *iPtr++;
          count = (int)(pixel & 0x7F);
          if (!count)
            return;
          if (pixel & 0x80) 
            {
              while (count--) 
                {
                  *oPtr++ = *iPtr++;
                }
            } 
          else 
            {
              pixel = *iPtr++;
              while (count--) 
                {
                  *oPtr++ = pixel;
                }
            }
        }
    }
  else /* verbatim image */
    {
      fseek(image->file, 512+(y*image->sizeX)+(z*image->sizeX*image->sizeY),
            SEEK_SET);
      cfread(buf, 1, image->sizeX, image->file, fileName);
    }
}

static void imageGetRawData( ImageType *image, char *data, const char *fileName)
{
  int i, j, k;
  int remain;

  switch ( image->sizeZ )
    {
    case 1:
      remain = image->sizeX % 4;
      break;
    case 2:
      remain = image->sizeX % 2;
      break;
    case 3:
      remain = (image->sizeX * 3) & 0x3;
      if (remain)
        remain = 4 - remain;
      break;
    case 4:
      remain = 0;
      break;
    }

  for (i = 0; i < image->sizeY; i++) 
    {
      for ( k = 0; k < image->sizeZ ; k++ )
        imageGetRow(image, image->tmp[k+1], i, k, fileName);
      for (j = 0; j < image->sizeX; j++) 
        for ( k = 1; k <= image->sizeZ ; k++ )
          *data++ = *(image->tmp[k] + j);
      data += remain;
    }
}

Image *loadImage(const char *fileName)
{
  ImageType *image;
  Image *final;
  int sx;

  image = imageOpen(fileName);

  final = (Image *)malloc(sizeof(Image));
  if (final == NULL) 
    {
      fprintf(stderr, "Out of memory!\n");
      exit(-1);
    }
  final->imagic = image->imagic;
  final->type = image->type;
  final->dim = image->dim;
  final->sizeX = image->sizeX; 
  final->sizeY = image->sizeY;
  final->sizeZ = image->sizeZ;

  /* 
   * Round up so rows are long-word aligned 
   */
  sx = ( (image->sizeX) * (image->sizeZ) + 3) >> 2;

  final->data 
    = (unsigned char *)malloc( sx * image->sizeY * sizeof(unsigned int));

  if (final->data == NULL) 
    {
      fprintf(stderr, "Out of memory!\n");
      exit(-1);
    }

  imageGetRawData(image, (char *)(final->data), fileName);
  imageClose(image);
  return final;
}

void deleteImage(Image *img)
{
  free(img->data);
  free(img);
}
