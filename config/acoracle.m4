dnl $Id$
dnl
dnl acoracle.m4
dnl
dnl Author: G.J. Paulissen (g.paulissen@chello.nl)
dnl
dnl Defines the macros:
dnl  ACX_SEARCH_LIBS
dnl  ACX_PROG_PROC
dnl  ACX_PROG_SQLPLUS
dnl


# ACX_SEARCH_LIBS(ROOT-DIR, SUB-DIRS, FUNCTION, SEARCH-LIBS [, ACTION-IF-FOUND
#            [, ACTION-IF-NOT-FOUND [, OTHER-LIBRARIES]]])
# Search for a library defining FUNC, if it's not already available.
#
# Similar to AC_SEARCH_LIBS but also adds the directory name where the library
# is found to LDFLAGS.
#
# This macro has been developed because the Oracle libraries on a Windows
# platform may reside in various subdirectories. For instance the orasql9
# library may be found in $ORACLE_HOME/precomp/lib/msvc, but also in
# $ORACLE_HOME/precomp/lib. On Unix systems the library is called clntsh and
# can be found in $ORACLE_HOME/lib.
#
# The Cygwin compiler is also able to link to DLLs on a Windows platform.
# However this will likely result in static libraries being built.
#
# The Iterix compiler (Windows Services For Unix from Microsoft) can not link
# to a .lib file via -l. In that case the full path must be added to
# LIBS before calling this macro.

AC_DEFUN([ACX_SEARCH_LIBS],
[AC_CACHE_CHECK([for directory and library containing $3], [acx_cv_search_$3],
[acx_func_search_save_LDFLAGS=$LDFLAGS
acx_func_search_save_LIBS=$LIBS
acx_cv_search_$3=no
AC_LINK_IFELSE([AC_LANG_CALL([], [$3])],
	       [acx_cv_search_$3="none required"])
if test "$acx_cv_search_$3" = no; then
  for acx_subdir in $2; do
    acx_dir="$1/$acx_subdir"
    LDFLAGS="-L$acx_dir $acx_func_search_save_LDFLAGS"
    for acx_lib in $4; do
      LIBS="-l$acx_lib $7 $acx_func_search_save_LIBS"
      AC_LINK_IFELSE([AC_LANG_CALL([], [$3])],
	             [acx_cv_search_$3="-L$acx_dir -l$acx_lib" && break])
    done
    test "$acx_cv_search_$3" = "no" || break
  done
fi
LDFLAGS=$acx_func_search_save_LDFLAGS
LIBS=$acx_func_search_save_LIBS])
AS_IF([test "$acx_cv_search_$3" != no],
      [if test "$acx_cv_search_$3" != "none required"
then 
  LDFLAGS="`eval echo \$acx_cv_search_$3 | cut -d' ' -f 1` $LDFLAGS"
  LIBS="`eval echo \$acx_cv_search_$3 | cut -d' ' -f 2` $LIBS"
fi
       $5],
      [$6])dnl
])

# ACX_PROG_PROC
# -------------
# Look for the Oracle PRO*C compiler. 
# Sets/updates the following variables:
# - PROC          the full path name of the PRO*C compiler
# - PROCFLAGS     PRO*C compiler flags.
#                 The -D.. flags of CPPFLAGS are converted into define=..
# - PROCINCLUDES  PRO*C compiler includes including directory
#                 of the PRO*C prototype header.
#                 The -I flags of CPPFLAGS are converted into include=..

AC_DEFUN([ACX_PROG_PROC],
[AC_PATH_PROG([PROC], [proc], [AC_MSG_ERROR(proc not found)], ["$ORACLE_HOME/bin:$PATH"])dnl

acx_oracle_home="$ORACLE_HOME"
if test -z "$acx_oracle_home"
then
  acx_oracle_home=`dirname $PROC`
  acx_oracle_home=`dirname $acx_oracle_home`
fi

ACX_SEARCH_LIBS([$acx_oracle_home],
                [lib precomp precomp/lib precomp/lib/msvc bin],
                [sqlglm],
                [clntsh orasql10 orasql9 orasql8 orasql7],
		[],
                [AC_MSG_ERROR(sqlglm not found)])
ACX_SEARCH_LIBS([$acx_oracle_home],
                [lib precomp precomp/lib precomp/lib/msvc bin],
                [osnsui],
                [clntsh oran10 oran9 oran8 oran7],
		[],
		[AC_MSG_ERROR(osnsui not found)])

acx_protohdrs="sqlcpr.h sqlproto.h"
acx_protohdr=
for dir in $acx_oracle_home/precomp/public $acx_oracle_home
do
  test -d $dir || continue
  for file in $acx_protohdrs 
  do
    # Windows: ignore case
    # Bug 849475: just return one header by using head -1
    acx_protohdr=`find $dir \( -name \*.h -o -name \*.H \) | grep -i $file | head -1 2>/dev/null`
    if test -n "$acx_protohdr"
    then
      CPPFLAGS="-I`dirname $acx_protohdr` $CPPFLAGS"
      break
    fi
  done
  # One is enough
  test -z "$acx_protohdr" || break
done

AC_CHECK_HEADERS([$acx_protohdrs], [break], [AC_MSG_ERROR(PRO*C prototype header not found)])

PROCINCLUDES='`echo " $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)" | sed "s/ -I/ INCLUDE=/g;s/ -[[^ \t]]*//g"`'
PROCFLAGS='`echo " $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)" | sed "s/ -D/ DEFINE=/g;s/ -[[^ \t]]*//g"`'

AC_SUBST(PROCINCLUDES)
PROCFLAGS="$PROCFLAGS CODE=ANSI_C PARSE=NONE SQLCHECK=FULL USERID=\$(USERID)"
AC_SUBST(PROCFLAGS)
])


# ACX_PROG_SQLPLUS
# ----------------
# Look for the Oracle SQL*Plus program in directory of PRO*C, ORACLE_HOME/bin or PATH.
# Sets/updates the following variables:
# - SQLPLUS       the full path name of the SQL*Plus program.

AC_DEFUN([ACX_PROG_SQLPLUS],
[AC_ARG_VAR([SQLPLUS], [Oracle SQL*Plus program])dnl

if test -n "${PROC:=}"
then
  acx_path="`dirname $PROC`:$PATH"
elif test -n "${ORACLE_HOME:=}"
then
  acx_path="$ORACLE_HOME/bin:$PATH"
else
  acx_path="$PATH"
fi

AC_PATH_PROGS([SQLPLUS], [plus80 plus33 plus32 plus31 sqlplus], [], [$acx_path])
test -n "$SQLPLUS" || AC_MSG_ERROR(sqlplus not found)
])


# ACX_PROG_XML
# -------------
# Look for the Oracle XML library and header.
# Sets/updates the following variables:
# - LIBS          extended with the XML library
# - CPPFLAGS      extended with the XML header

AC_DEFUN([ACX_PROG_XML],
[
acx_oracle_home="$ORACLE_HOME"
if test -z "$acx_oracle_home"
then
  acx_oracle_home=`dirname $PROC`
  acx_oracle_home=`dirname $acx_oracle_home`
fi

ACX_SEARCH_LIBS([$acx_oracle_home],
                [lib bin],
                [xmlinit],
                [xml10 oraxml10 oraxml9 oraxml8],
		[],
		[AC_MSG_ERROR(xmlinit not found)])

acx_xmlhdrs="oraxml.h"
#acx_xmlhdrs="oraxml.h oratypes.h"
acx_xmlhdr=
for dir in $acx_oracle_home/xdk/include $acx_oracle_home/xdk/c/parser/include $acx_oracle_home
do
  test -d $dir || continue
  for file in $acx_xmlhdrs
  do
    # Windows: ignore case
    # Bug 849475: just return one header by using head -1
    acx_xmlhdr=`find $dir \( -name \*.h -o -name \*.H \) | grep -i $file | head -1 2>/dev/null`
    if test -n "$acx_xmlhdr"
    then
      CPPFLAGS="-I`dirname $acx_xmlhdr` $CPPFLAGS"
      break
    fi
  done
  test -z "$acx_xmlhdr" || break
done

AC_CHECK_HEADERS([$acx_xmlhdrs], [continue], [AC_MSG_ERROR(XML header not found)])
])

dnl $Id$
