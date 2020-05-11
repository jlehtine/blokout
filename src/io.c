/*************************************************************************
 * Copyright (C) 2020 Johannes Lehtinen
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
#include "io.h"

void cfread(void *ptr, size_t size, size_t nmemb, FILE *stream, const char *fileName) {
  size_t res;

  res = fread(ptr, size, nmemb, stream);
  if (res != nmemb)
    {
      const char *err =
        ferror(stream) ?
        "Error reading file: %s\n" :
        "Unexpected end of file: %s\n";
      fprintf(stderr, err, fileName);
      exit(-1);
    }
}
