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

#ifndef __MENU_H_INCLUDED__
#define __MENU_H_INCLUDED__

#define MAIN_MENU 0
#define HELP_MENU 1
#define SETTINGS_MENU 2
#define HIGH_SCORE 3
#define BLOCK_SELECT 4
#define CONTINUE_GAME 5
#define END_GAME 6
#define NEW_GAME 7
#define QUIT 8

#define LEVEL 1
#define HEIGHT 2
#define XWIDTH 3
#define YWIDTH 4
#define SOLID_BLOCKS 5
#define BG_COLOR 6
#define SIDEBAR 7
#define SIDEBAR_TEXTURES 8

#if HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#if HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif

void initMenu(void);
void initSettingsMenu(void);
void menuAction(int selection);
void drawSelectionOverlay(int no);
void drawMenuOverlay(int menuNo, int selection);
void menuSpecial(int key, int x, int y);
void menuKeyboard(unsigned char key, int x, int y);
void menuMode(void);

extern int currentMenu;
extern int selection;

#endif
