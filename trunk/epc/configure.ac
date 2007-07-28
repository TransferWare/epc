# -*-fundamental-*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2002 G.J. Paulissen
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

AC_REVISION([$Revision$])
AC_INIT([epc],[4.6.0],[g.paulissen@chello.nl])

# version check
if test -f "$srcdir/VERSION"
then
  read version < $srcdir/VERSION
else
  version=
fi

if test "$version" != "$PACKAGE_VERSION"
then
  echo "Updating file $srcdir/VERSION with package version $PACKAGE_VERSION." 1>&2
  echo "$PACKAGE_VERSION" > $srcdir/VERSION
fi

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([test/main.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_C_PROTOTYPES

# Checks for programs.
AC_PATH_PROG([PERL],[perl])
case x$PERL in
x)
	# No Perl avaliable: stop
	AC_MSG_ERROR([Perl must be available in \$PATH.])
	;;
esac
dnl AC_ARG_VAR(PERL,[The program name of Perl.])
AC_PROG_CC
AM_PROG_LEX
if test "$LEX" != flex; then
  LEX="$SHELL $missing_dir/missing flex"
  AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
  AC_SUBST(LEXLIB, '')
fi
AC_PROG_YACC
AM_PROG_CC_STDC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# pldoc generates documentation from Oracle package specification files.
case "$ac_cv_host" in
*-mingw32)
	pldoc=pldoc.bat
	;;
*-cygwin)
	pldoc=pldoc.bat
	;;
*)
	pldoc=pldoc.sh
	;;
esac

AC_CHECK_PROGS([PLDOC], [$pldoc pldoc], [])

# doxygen generates documentation from C sources.
AC_CHECK_PROGS([DOXYGEN], [doxygen], [])

# Oracle checks
ACX_PROG_PROC
ACX_PROG_SQLPLUS
ACX_PROG_XML

# Check for _int64, __int64, unsigned long long used by Oracle header files oratypes.h and oraxml.h
AC_CHECK_TYPES([_int64, __int64, unsigned long long])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([assert.h malloc.h stdlib.h signal.h string.h sys/types.h time.h stdarg.h])
AC_CHECK_HEADERS([sqlproto.h sqlcpr.h oraxml.h oratypes.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strstr strdup])
LIBS="-ldbug $LIBS"
AC_CHECK_FUNC([dbug_enter],[],[AC_MSG_ERROR(dbug_enter not found)])

# This is necessary so that .o files in LIBOBJS are also built via
# the ANSI2KNR-filtering rules.
LIB@&t@OBJS=`echo "$LIB@&t@OBJS" |
             sed 's,\.[[^.]]* ,$U&,g;s,\.[[^.]]*$,$U&,'`
LTLIBOBJS=`echo "$LIB@&t@OBJS" |
           sed 's,\.[[^.]]* ,.lo ,g;s,\.[[^.]]*$,.lo,'`
AC_SUBST(LTLIBOBJS)dnl
AC_SUBST(ac_aux_dir)dnl
AC_CONFIG_FILES([src/Makefile src/lib/Makefile src/prog/Makefile src/sql/Makefile])
AC_CONFIG_FILES([Makefile test/Makefile])
AC_CONFIG_FILES([examples/Makefile examples/demo/Makefile examples/soap/Makefile examples/xmlrpc/Makefile])
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


