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
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "timing.h"

void resetTimer(Timer *timer)
{
  /* Reset timer values */
  timer->isRunning = 0;
  timer->cumulativeTime = 0;
}

void startTimer(Timer *timer)
{
  /* Do nothing if already running */
  if(timer->isRunning)
    return;

  /* Update timer status */
  if(gettimeofday(&(timer->lastUpdate), NULL)) {
    perror("gettimeofday()");
    exit(1);
  }
  timer->isRunning = 1;
}

void stopTimer(Timer *timer)
{
  struct timeval timeNow;
  
  /* Do nothing if already stopped */
  if(! timer->isRunning)
    return;

  /* Update timer status */
  if(gettimeofday(&timeNow, NULL)) {
    perror("gettimeofday()");
    exit(1);
  }
  timer->cumulativeTime +=
    (float)(timeNow.tv_sec - timer->lastUpdate.tv_sec) +
    (float)(timeNow.tv_usec - timer->lastUpdate.tv_usec) / 1000000.0;
  timer->isRunning = 0;
}

float readTimer(Timer *timer)
{
  struct timeval timeNow;

  /* Just return the time if timer is not running... */
  if(! timer->isRunning)
    return timer->cumulativeTime;

  /* ...otherwise calculate the current time */
  if(gettimeofday(&timeNow, NULL)) {
    perror("gettimeofday()");
    exit(1);
  }
  return timer->cumulativeTime +
    (float)(timeNow.tv_sec - timer->lastUpdate.tv_sec) +
    (float)(timeNow.tv_usec - timer->lastUpdate.tv_usec) / 1000000.0;
}
