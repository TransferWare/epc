# -*-fundamental-*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2002 G.J. Paulissen
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Strip RCS keyword Name (e.g. '$Name$' into 2_1_0).
AC_DEFUN(AC_RCS_NAME, translit(regexp([$Name$], [epc_\(.*\) ], [\1]), [_], [.]))

AC_REVISION([$Revision$])
AC_INIT([epc],[AC_RCS_NAME()],[g.paulissen@chello.nl])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([test/main.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_C_PROTOTYPES

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
if test "$LEX" != flex; then
  LEX="$SHELL $missing_dir/missing flex"
  AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
  AC_SUBST(LEXLIB, '')
fi
AC_PROG_YACC
AM_PROG_CC_STDC
# Disable shared library building on Windows
# because using the Oracle import library results in linker warnings
#case "$host" in
#*-*-cygwin* | *-*-mingw* )
#  AC_DISABLE_SHARED
#  ;;
#esac
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Oracle checks
ACX_PROG_PROC
ACX_PROG_SQLPLUS

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([assert.h malloc.h stdlib.h signal.h string.h sys/types.h time.h stdarg.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strstr strdup])

# This is necessary so that .o files in LIBOBJS are also built via
# the ANSI2KNR-filtering rules.
LIB@&t@OBJS=`echo "$LIB@&t@OBJS" |
             sed 's,\.[[^.]]* ,$U&,g;s,\.[[^.]]*$,$U&,'`
LTLIBOBJS=`echo "$LIB@&t@OBJS" |
           sed 's,\.[[^.]]* ,.lo ,g;s,\.[[^.]]*$,.lo,'`
AC_SUBST(LTLIBOBJS)dnl
AC_SUBST(ac_aux_dir)dnl
AC_CONFIG_FILES([src/Makefile src/lib/Makefile src/prog/Makefile src/sql/Makefile])
AC_CONFIG_FILES([Makefile config/Makefile test/Makefile])
AC_CONFIG_FILES([examples/Makefile examples/demo/Makefile])
AC_CONFIG_FILES([doc/Makefile utils/Makefile])dnl

AC_OUTPUT

echo \
"------------------------------------------------------------------------
Configuration:

  Source code location:       ${srcdir}
  Compiler:                   ${CC}
  Compiler flags:             ${CFLAGS}
  Host System Type:           ${host}
  Install path:               ${prefix}

  See config.h for further configuration information.
------------------------------------------------------------------------"


