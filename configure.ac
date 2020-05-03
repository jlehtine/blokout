dnl Process this file with autoconf to produce a configure script.

# -----------------------
# Autoconf initialization
# -----------------------
AC_INIT([Blokout], [0.1.3], [johannes.lehtinen@iki.fi], [blokout])
AC_COPYRIGHT([Copyright 2005 Johannes Lehtinen
This configure script is free software; Johannes Lehtinen gives unlimited
permission to copy, distribute and modify it.])
AC_CONFIG_SRCDIR([src/highscore.c])

# -----------------------
# Automake initialization
# -----------------------
AM_INIT_AUTOMAKE([foreign])

# ----------
# C settings
# ----------
AC_LANG([C])
AC_PROG_CC
AC_PROG_CC_C_O
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# --------------
# Check for host
# --------------
AC_CANONICAL_HOST

# -----------------------------------------
# Check if standard warnings should be used
# -----------------------------------------
AC_ARG_ENABLE([gcc-warnings],
  AS_HELP_STRING([--enable-gcc-warnings], [Enable standard GCC warnings]),
  [if test "$enableval" != no; then
    CFLAGS="$CFLAGS -Wall -pedantic -std=c99"
  fi])

# ----------------------
# Check for math library
# ----------------------
AC_SEARCH_LIBS([sin], [m])

# -----------------------------
# Check for X headers/libraries
# -----------------------------
AC_PATH_XTRA
if test "$no_x" != yes; then
  bo_save_CFLAGS="$CFLAGS"
  bo_save_LIBS="$LIBS"
  bo_save_LDFLAGS="$LDFLAGS"
  CFLAGS="$CFLAGS $X_CFLAGS"
  LIBS="$X_PRE_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
  LDFLAGS="$LDFLAGS $X_LIBS"
  AC_CHECK_FUNC([XOpenDisplay],,
    [AC_MSG_NOTICE([Disabling X])
    CFLAGS="$bo_save_CFLAGS"
    LIBS="$bo_save_LIBS"
    LDFLAGS="$bo_save_LDFLAGS"])
fi

# -------------------------------
# Check for Posix thread library
# (might be needed by GL library)
# -------------------------------
AC_SEARCH_LIBS([pthread_key_create], [pthread])

# ------------------------------------
# Check for OpenGL headers/libraries
# ------------------------------------
AC_ARG_WITH([gl-includes],
  AS_HELP_STRING([--with-gl-includes=DIR], [OpenGL header files are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    CPPFLAGS="$CPPFLAGS -I$withval"
  fi])
AC_CHECK_HEADER([GL/gl.h],,
  AC_MSG_ERROR([OpenGL header files are required.]))

AC_ARG_WITH([gl-libraries],
  AS_HELP_STRING([--with-gl-libraries=DIR], [OpenGL libraries are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    LDFLAGS="$LDFLAGS -L$withval"
  fi])
AC_SEARCH_LIBS_PROG([glEnable], [GL opengl opengl32],
                    [#include <GL/gl.h>],
                    [glEnable(GL_BLEND);],,
                    [AC_MSG_ERROR([GL library is required.])])
AC_SEARCH_LIBS_PROG([gluLookAt], [GLU glu glu32], 
                    [
#include <GL/gl.h>
#include <GL/glu.h>
],
                    [gluLookAt(0, 0, 0, 0, 0, 0, 0, 0, 0);],,
                    [AC_MSG_ERROR([GLU library is required.])])

# ---------------------------------
# Check for Xmu library
# (might be needed by glut library)
# ---------------------------------
AC_SEARCH_LIBS([XmuLookupStandardColormap], [Xmu])

# --------------------------------
# Check for GLUT headers/libraries
# --------------------------------
AC_ARG_WITH([glut-includes],
  AS_HELP_STRING([--with-glut-includes=DIR], [GLUT header files are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    CPPFLAGS="$CPPFLAGS -I$withval"
  fi])
found_glut_headers=false
AC_CHECK_HEADERS([GL/glut.h GL/freeglut.h], [found_glut_headers=true; break])
if test $found_glut_headers = false; then
  AC_MSG_ERROR([GLUT header files are required.]);
fi

AC_ARG_WITH([glut-libraries],
  AS_HELP_STRING([--with-glut-libraries=DIR], [GLUT libraries are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    LDFLAGS="$LDFLAGS -L$withval"
  fi])
AC_SEARCH_LIBS_PROG([glutPostRedisplay], [glut glut32 freeglut],
                    [
#include <GL/gl.h>
#ifdef HAVE_GL_GLUT_H
#include <GL/glut.h>
#endif
#ifdef HAVE_GL_FREEGLUT_H
#include <GL/freeglut.h>
#endif
],
                     [glutPostRedisplay();],,
                     [AC_MSG_ERROR([GLUT library is required.])])

# ------------------------------
# Check for missing gettimeofday
# (this is for Windows support)
# ------------------------------
AC_CHECK_FUNC([gettimeofday],,
	[AC_LIBOBJ([gettimeofday])
	AC_DEFINE([BO_GETTIMEOFDAY])
    # Windows uses a calling convention which requires includes
    AC_MSG_CHECKING([for timeGetTime in -lwinmm])
	LIBS="-lwinmm $LIBS"
	AC_LINK_IFELSE([AC_LANG_SOURCE([
#include <windows.h>
#include <time.h>
int main(int argc, char *argv@<:@@:>@) {
  timeGetTime();
  return 0;
}
])], AC_MSG_RESULT([yes]),
  [AC_MSG_RESULT([no])
  AC_MSG_ERROR([Either gettimeofday or timeGetTime is required.])])])

# ---------------------------------------
# Check the path separator character used
# ---------------------------------------
case "$host" in
  *windows*|*mingw*)
    bopathsep='\\\\'
    ;;
  *)
    bopathsep='/'
    ;;
esac
BOPATHSEP='\"'"$bopathsep"'\"'
AC_SUBST(BOPATHSEP)

# -------------------------------------------------------------------
# Check if data directory is disabled (use working directory instead)
# -------------------------------------------------------------------
AC_ARG_ENABLE([cwd-data],
  AS_HELP_STRING([--enable-cwd-data],
    [load data from the current working directory instead of using absolute data directory (default for Windows)]))
if test "$enable_cwd_data" = yes || test "$bopathsep" != /; then
  BODATADIR='\"data\"'
else
  BODATADIR='\"$(datadir)/$(PACKAGE)\"'
fi
AC_SUBST(BODATADIR)

# ------------------
# Write output files
# ------------------
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  data/Makefile
])
AC_OUTPUT

# ---------------------
# Display configuration
# ---------------------
AC_MSG_NOTICE([------------------------------------------------------------])
AC_MSG_NOTICE([Blokout configuration])
AC_MSG_NOTICE([  CC='$CC'])
AC_MSG_NOTICE([  CPPFLAGS='$CPPFLAGS'])
AC_MSG_NOTICE([  CFLAGS='$CFLAGS'])
AC_MSG_NOTICE([  LDFLAGS='$LDFLAGS'])
AC_MSG_NOTICE([  LIBS='$LIBS'])
AC_MSG_NOTICE([  BODATADIR='$BODATADIR'])
AC_MSG_NOTICE([  BOPATHSEP='$BOPATHSEP'])
AC_MSG_NOTICE([  prefix='$prefix'])
AC_MSG_NOTICE([  exec_prefix='$prefix'])
AC_MSG_NOTICE([  bindir='$bindir'])
AC_MSG_NOTICE([  datadir='$datadir'])
AC_MSG_NOTICE([------------------------------------------------------------])
