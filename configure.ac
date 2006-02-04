dnl Process this file with autoconf to produce a configure script.
dnl
dnl $Id$

# -----------------------
# Autoconf initialization
# -----------------------
AC_INIT([Blokout], [0.1.1], [johannes.lehtinen@iki.fi], [blokout])
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
AC_CHECK_LIB([m], [sin])

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
AC_CHECK_LIB([pthread], [pthread_key_create])

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
AC_CHECK_LIB([GL], [glEnable],,
  AC_MSG_ERROR([GL library is required.]))
AC_CHECK_LIB([GLU], [gluLookAt],,
  AC_CHECK_LIB([MesaGLU], [gluLookAt],,
    AC_CHECK_LIB([glu32], [gluLookAt],,
      AC_MSG_ERROR([GLU or MesaGLU library is required.]))))

# ---------------------------------
# Check for Xmu library
# (might be needed by glut library)
# ---------------------------------
AC_CHECK_LIB([Xmu], [XmuLookupStandardColormap])

# --------------------------------
# Check for GLUT headers/libraries
# --------------------------------
AC_ARG_WITH([glut-includes],
  AS_HELP_STRING([--with-glut-includes=DIR], [GLUT header files are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    CPPFLAGS="$CPPFLAGS -I$withval"
  fi])
AC_CHECK_HEADER([GL/glut.h],,
  AC_MSG_ERROR([GLUT header files are required.]))

AC_ARG_WITH([glut-libraries],
  AS_HELP_STRING([--with-glut-libraries=DIR], [GLUT libraries are in DIR]),
  [if test "$withval" != yes && test "$withval" != no; then
    LDFLAGS="$LDFLAGS -L$withval"
  fi])
AC_CHECK_LIB([glut], [glutPostRedisplay],,
  AC_MSG_ERROR([GLUT library is required.]))

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
AC_MSG_NOTICE([------------------------------------------------------------])
