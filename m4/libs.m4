# Blokout
# Copyright 2000 Johannes Lehtinen
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# You should have received a copy of the GNU General Public License
# along with this program; see file COPYINGv3.
# If not, see <http://www.gnu.org/licenses/>.

# AC_SEARCH_LIBS_PROG(FUNCTION, SEARCH-LIBS, PROLOGUE, BODY,
#                     [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#                     [OTHER-LIBRARIES])
#
# Modified from GNU Autoconf 2.69 AC_SEARCH_LIBS by Johannes Lehtinen.
#
# Like AC_SEARCH_LIBS but uses AC_LANG_PROGRAM(PROLOGUE, BODY) to check
# availability of the library. This is used to check availability of
# libraries in compilation environments that require header files for
# the linking to be successful, e.g. for Windows cross-compilation.
AC_DEFUN([AC_SEARCH_LIBS_PROG],
[AS_VAR_PUSHDEF([ac_Search], [ac_cv_search_$1])dnl
AC_CACHE_CHECK([for library containing $1], [ac_Search],
[ac_func_search_save_LIBS=$LIBS
AC_LANG_CONFTEST([AC_LANG_CALL([], [$1])])
for ac_lib in '' $2; do
  if test -z "$ac_lib"; then
    ac_res="none required"
  else
    ac_res=-l$ac_lib
    LIBS="-l$ac_lib $7 $ac_func_search_save_LIBS"
  fi
  AC_LINK_IFELSE([AC_LANG_PROGRAM([$3], [$4])], [AS_VAR_SET([ac_Search], [$ac_res])])
  AS_VAR_SET_IF([ac_Search], [break])
done
AS_VAR_SET_IF([ac_Search], , [AS_VAR_SET([ac_Search], [no])])
LIBS=$ac_func_search_save_LIBS])
AS_VAR_COPY([ac_res], [ac_Search])
AS_IF([test "$ac_res" != no],
  [test "$ac_res" = "none required" || LIBS="$ac_res $LIBS"
  $5],
      [$6])
AS_VAR_POPDEF([ac_Search])dnl
])
