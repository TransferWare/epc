dnl $Id$
dnl
dnl acoracle.m4
dnl
dnl Author: Kai Poitschke <kai.poitschke@computer.org>
dnl
dnl Defines the macros:
dnl  ACX_LD_RUNPATH_SWITCH 
dnl  AM_PATH_ORACLE
dnl


# ACX_PROG_PROC
# -------------
# Look for the Oracle PRO*C compiler. 
# Sets/updates the following variables:
# - PROC          the full path name of the PRO*C compiler
# - PROCLIB       its associated library (-L<libclntsh directory> -lclntsh for Unix)
# - PROCFLAGS     PRO*C compiler flags.
#                 The -D.. flags of CPPFLAGS are converted into define=..
# - PROCINCLUDES  PRO*C compiler includes including directory
#                 of the PRO*C prototype header.
#                 The -I flags of CPPFLAGS are converted into include=..

#AC_DEFUN([ACX_PROG_PROC],
#[AC_REQUIRE(AC_CANONICAL_HOST)dnl

AC_DEFUN([ACX_PROG_PROC],
[AC_PATH_PROG([PROC], [proc], [AC_MSG_ERROR(proc not found)])dnl

acx_cv_oracle_home="$ORACLE_HOME"
if test -z "$acx_cv_oracle_home"
then
  acx_cv_oracle_home=`dirname $PROC`
  acx_cv_oracle_home=`dirname $acx_cv_oracle_home`
fi

AC_MSG_CHECKING(for the full path name of the PRO*C library)
#set -x
PROCLIB=
case "$host" in
*-*-cygwin* | *-*-mingw* )
  # Windows
  AC_MSG_CHECKING(for the Windows PRO*C library)
  for dir in $acx_cv_oracle_home/precomp/lib/msvc $acx_cv_oracle_home
  do
    for base in orasql9 orasql8 sqllib18
    do
      # Windows: ignores case
      PROCLIB=`find $dir \( -name \*.lib -o -name \*.LIB \) | grep -i $base. 2>/dev/null`
      if test -n "$PROCLIB"
      then
        PROCLIB="-L`dirname $PROCLIB` -l$base"
        break;
      fi
    done
    test -z "$PROCLIB" || break
  done
  ;;
* )
  # Unix
  AC_MSG_CHECKING(for the Unix PRO*C library)
  for dir in $acx_cv_oracle_home/lib $acx_cv_oracle_home
  do
    for base in clntsh
    do
      PROCLIB=`find $dir -name lib$base.\* 2>/dev/null`
      if test -n "$PROCLIB"
      then
        PROCLIB="-L`dirname $PROCLIB` -l$base"
        break;
      fi
    done
    test -z "$PROCLIB" || break
  done
  ;;
esac
acx_save_LIBS="$LIBS"
LIBS="$PROCLIB $LIBS"
AC_TRY_LINK_FUNC(sqlglm, [], [AC_MSG_ERROR(PRO*C library not found)])
LIBS=$acx_save_LIBS
AC_MSG_RESULT($PROCLIB)
AC_SUBST(PROCLIB)

AC_MSG_CHECKING(for the PRO*C prototype header)
acx_cv_prochdr=
for dir in $acx_cv_oracle_home/precomp/public $acx_cv_oracle_home
do
  for file in sqlcpr.h sqlproto.h
  do
    # Windows: ignores case
    acx_cv_protohdr=`find $dir \( -name \*.h -o -name \*.H \) | grep -i $file 2>/dev/null`
    test -z "$acx_cv_protohdr" || break
  done
  test -z "$acx_cv_protohdr" || break
done

CPPFLAGS="-I`dirname $acx_cv_protohdr` $CPPFLAGS"
acx_cv_protohdr=`basename $acx_cv_protohdr`
AC_CHECK_HEADERS([sqlcpr.h sqlproto.h], [break], [AC_MSG_ERROR(PRO*C prototype header not found)])
AC_MSG_RESULT($acx_cv_protohdr)

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
  acx_cv_path=`dirname $PROC`
elif test -n "${ORACLE_HOME:=}"
then
  acx_cv_path="$ORACLE_HOME/bin"
else
  acx_cv_path="$PATH"
fi

AC_PATH_PROGS([SQLPLUS], [plus80 plus33 plus32 plus31 sqlplus], [], [$acx_cv_path])
test -n "$SQLPLUS" || AC_MSG_ERROR(sqlplus not found)
])

dnl $Id$
