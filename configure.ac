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
AC_INIT([epc],[5.2.0],[gert.jan.paulissen@gmail.com])

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

AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([test/main.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_HOST

case ${host} in
*-cygwin)
  if gcc --version | grep -E "gcc \(GCC\) [1-3]\." 1>/dev/null 2>&1
  then
          # gcc (GCC) 1.x or 2.x or 3.x: use -mno-cygwin
          test "$CC" = "" && CC="gcc -mno-cygwin" && export CC
  fi
  ;;
*solaris*|*sun*)
  test "$LIBS" = "" && LIBS="-lm" && export LIBS
  ;;
?*)
  ;;
*)
  AC_MSG_ERROR([Host not set])
  ;;
esac

AM_INIT_AUTOMAKE

# AM_C_PROTOTYPES
AM_WITH_DMALLOC

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
AC_PATH_PROG([LEX], [$LEX], [])
if test -z "$LEX"; then
  AC_MSG_ERROR([Please install flex, lex or one of their variants])
fi

AC_PROG_YACC
# if $YACC equals yacc, the program may not exist...
if test "$YACC" = "yacc"; then
  AC_PATH_PROG([YACC], [yacc], [])
  if test -z "$YACC"; then
    AC_MSG_ERROR([Please install bison, yacc or one of their variants])
  fi
fi

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
AM_CONDITIONAL([PLDOC], [test -n "$PLDOC"])

# doxygen generates documentation from C sources.
AC_CHECK_PROGS([DOXYGEN], [doxygen], [])
AM_CONDITIONAL([DOXYGEN], [test -n "$DOXYGEN"])

# dbug package ?
ACX_WITH_DBUG

# Oracle checks
ACX_PROG_PROC
ACX_PROG_SQLPLUS
AC_ARG_ENABLE([xml],
              AS_HELP_STRING([--enable-xml],
                             [enable XML processing [[default=no]]]),
              [case "$enableval" in
                 yes) acx_cv_xml=true ;;
                 no)  acx_cv_xml=false ;;
                 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-xml]) ;;
               esac],
              [acx_cv_xml=false])
AM_CONDITIONAL([XML], [test "$acx_cv_xml" == "true"])
if [ "$acx_cv_xml" == "true" ]
then
  ACX_PROG_XML
else
  AC_DEFINE([XML_OFF],[1],[Define if NOT using XML])
fi

#
# The checks for LIBTOOL must be placed after ACX_PROG_PROC, since
# that macro changes LDFLAGS and LIBS which affects the libtool settings
# on AIX.
#
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

LT_INIT

# Check for _int64, __int64, unsigned long long used by Oracle header files oratypes.h and oraxml.h
AC_CHECK_TYPES([_int64, __int64, unsigned long long])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([assert.h errno.h malloc.h stdlib.h signal.h string.h sys/types.h time.h stdarg.h])
AC_CHECK_HEADERS([sqlproto.h sqlcpr.h oraxml.h oratypes.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CONFIG_LIBOBJ_DIR([replace])
AC_FUNC_MALLOC
AC_REPLACE_FUNCS([strtof])
AC_LIBOBJ([malloc])
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strstr strdup])

AC_ARG_ENABLE([server-interrupt],
              AS_HELP_STRING([--enable-server-interrupt],
                             [enable interrupting the server [[default=yes]]]),
              [case "$enableval" in
                 yes) acx_cv_use_server_interrupt=true ;;
                 no)  acx_cv_use_server_interrupt=false ;;
                 *)   AC_MSG_ERROR([bad value ${enableval} for --enable-server-interrupt]) ;;
               esac],
              [acx_cv_use_server_interrupt=true])

if test x$acx_cv_use_server_interrupt = xtrue
then
  AC_DEFINE([SERVER_INTERRUPT], [1], [Is the server interruptable?])
fi

# This is necessary so that .o files in LIBOBJS are also built via
# the ANSI2KNR-filtering rules.
LIB@&t@OBJS=`echo "$LIB@&t@OBJS" |
             sed 's,\.[[^.]]* ,$U&,g;s,\.[[^.]]*$,$U&,'`
LTLIBOBJS=`echo "$LIB@&t@OBJS" |
           sed 's,\.[[^.]]* ,.lo ,g;s,\.[[^.]]*$,.lo,'`
AC_SUBST(LTLIBOBJS)dnl
AC_SUBST(ac_aux_dir)dnl
AC_CONFIG_FILES([replace/Makefile])
AC_CONFIG_FILES([src/Makefile src/lib/Makefile src/prog/Makefile src/sql/Makefile])
# Perl is substituted in these scripts
AC_CONFIG_FILES([src/prog/epcdaemon.pl])
AC_CONFIG_FILES([Makefile test/Makefile])
# GJP 2018-08-20  Trick to only distribute it and not build (see also Makefile.am)
AM_CONDITIONAL([EXAMPLES], [false])
AC_CONFIG_FILES([examples/Makefile examples/demo/Makefile examples/soap/Makefile examples/xmlrpc/Makefile])
AC_CONFIG_FILES([doc/Makefile doc/Doxyfile utils/Makefile])dnl

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
