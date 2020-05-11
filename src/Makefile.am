## Process this file with automake to produce Makefile.in
## $Id$

AM_CPPFLAGS = -DBODATADIR=@BODATADIR@ -DBOPATHSEP=@BOPATHSEP@

bin_PROGRAMS = blokout
blokout_LDADD = $(LIBOBJS)

blokout_SOURCES = blocks.c blocks.h collision.c collision.h \
	control.c control.h counter.c counter.h forms.c forms.h \
	game.c game.h highscore.c highscore.h highscoreui.c highscoreui.h \
	image.c image.h io.c io.h main.c menu.c menu.h overlay.c overlay.h \
	select.c select.h texture.c texture.h timing.c timing.h \
	transforms.c transforms.h tube.c tube.h
