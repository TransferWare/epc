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
    for base in orasql8 sqllib18
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

acx_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="-I`dirname $acx_cv_protohdr` $CPPFLAGS"
acx_cv_protohdr=`basename $acx_cv_protohdr`
AC_CHECK_HEADERS([sqlcpr.h sqlproto.h], [break], [AC_MSG_ERROR(PRO*C prototype header not found)])
AC_MSG_RESULT($acx_cv_protohdr)

set dummy $CPPFLAGS
for f
do
  case $f in
  -I*)
    PROCINCLUDES="$PROCINCLUDES `echo $f | sed -e 's/-I/include=/g'`"
    ;;
  -D*)
    PROCFLAGS="$PROCFLAGS `echo $f | sed -e 's/-D/define=/g'`"
    ;;
  esac
done
CPPFLAGS="$acx_save_CPPFLAGS"
AC_SUBST(PROCINCLUDES)
PROCFLAGS="$PROCFLAGS code=ansi_c parse=none sqlcheck=full userid=\$(USERID)"
AC_SUBST(PROCFLAGS)
])


# ACX_PROG_SQLPLUS
# ----------------
# Look for the Oracle SQL*Plus program.
# Sets/updates the following variables:
# - SQLPLUS       the full path name of the SQL*Plus program.

AC_DEFUN([ACX_PROG_SQLPLUS],
[AC_ARG_VAR([SQLPLUS], [Oracle SQL*Plus program])dnl
AC_PATH_PROGS([SQLPLUS], [plus80 plus33 plus32 plus31 sqlplus])
test -n "$SQLPLUS" || AC_MSG_ERROR(sqlplus not found)
])


dnl -------------------------------------------------------------------------
dnl ACX_LD_RUNPATH_SWITCH([ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
dnl Sets the variable LD_RUNPATH_SWITCH
dnl We try:
dnl -Wl,-R 
dnl -Wl,-rpath 
dnl -Wl,+s,+b
dnl If nothing works, the LD_RUNPATH_SWITCH is set to "".
dnl
AC_DEFUN(ACX_LD_RUNPATH_SWITCH, [

  if test "x$LD_RUNPATH_SWITCH" = "x"; then
    AC_CACHE_CHECK([if compiler supports -Wl,-R], acx_cv_cc_dashr,[
    acx_save_libs="${LIBS}"
    LIBS="-R/usr/lib ${LIBS}"
    AC_TRY_LINK([], [], acx_cv_cc_dashr=yes, acx_cv_cc_dashr=no)
    LIBS="${acx_save_libs}"])
  

    if test $acx_cv_cc_dashr = "yes"; then
        LD_RUNPATH_SWITCH="-Wl,-R"
    else
        AC_CACHE_CHECK([if compiler supports -Wl,-rpath,], acx_cv_cc_rpath,[
                acx_save_libs="${LIBS}"
                LIBS="-Wl,-rpath,/usr/lib ${LIBS}"
                AC_TRY_LINK([], [], acx_cv_cc_rpath=yes, acx_cv_cc_rpath=no)
                LIBS="${acx_save_libs}"])
        if test $acx_cv_cc_rpath = "yes"; then
            LD_RUNPATH_SWITCH="-Wl,-rpath,"
        else
            AC_CACHE_CHECK([if compiler supports -Wl,+s,+b,], acx_cv_cc_plusb,[
                acx_save_libs="${LIBS}"
                LIBS="-Wl,+s,+b,/usr/lib ${LIBS}"
                AC_TRY_LINK([], [], acx_cv_cc_plusb=yes, acx_cv_cc_plusb=no)
                LIBS="${acx_save_libs}"])

            if test $acx_cv_cc_plusb = "yes" ; then
                LD_RUNPATH_SWITCH="-Wl,+s,+b,"
            else
                LD_RUNPATH_SWITCH=""
            fi
        fi
    fi
  fi
  if test "x$LD_RUNPATH_SWITCH" != "x"; then
      # action-if-found
      ifelse([$1], , :, [$1])
  else
      # action-if-not-found
      ifelse([$2], , :, [$2])
  fi
])


dnl -------------------------------------------------------------------------
dnl ACX_ORACLE_VERSION([MINIMUM-VERSION [,ORACLE_HOME] [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Tries to find out which oracle version is installed
dnl Returns the version in $ORACLE_VERSION
dnl Substitutes ORACLE_VERSION
dnl
AC_DEFUN(ACX_ORACLE_VERSION, [

AC_ARG_ENABLE(oraversion, 
  AC_HELP_STRING([--disable-oraversion], [Do not check for the minimum oracle version]),
  [], [enable_oraversion=yes])

  ora_min_version=$1

  AC_CACHE_CHECK([Oracle version >= $ora_min_version], acx_cv_oracle_version, [

  acx_orahome=ifelse([$2], , $ORACLE_HOME, $2)

  export acx_orahome
  acx_cv_oracle_version=""

  if test -f "$acx_orahome/bin/sqlplus" ; then
     # Oracle 8i knows sqlplus -?
     # and Oracle 9i knows only sqlplus -v 
     # Cannot use the return code, have to parse the output
     n=`$acx_orahome/bin/sqlplus -? | grep -i -c Usage`
     if test $n -eq 0 ; then
       sqlplus_opt="-?"
     else
       sqlplus_opt="-v"
     fi
     acx_cv_oracle_version=`$acx_orahome/bin/sqlplus $sqlplus_opt |\
                    egrep '(Release|Version)' |\
                    sed 's/  */:/g' | cut -d: -f4 | cut -c 1-7`
  elif test -f "$acx_orahome/bin/svrmgrl" ; then
     acx_cv_oracle_version=`$acx_orahome/bin/svrmgrl command=exit |\
              egrep '(PL/SQL|JServer) (Release|Version)' |\
              sed 's/  */:/g' | cut -d: -f3 | cut -c 1-7`
  fi

  unset acx_orahome
  unset sqlplus_opt
  ])
  ORACLE_VERSION=$acx_cv_oracle_version
  AC_SUBST(ORACLE_VERSION)

  if test "x$ORACLE_VERSION" != "x"; then
      if test "x$enable_oraversion" = "xyes" ; then
          # split the minumum
          ora_min_major=`echo $ora_min_version. | cut -d. -f1`
          test -z "$ora_min_major" && ora_min_major=0
          ora_min_minor=`echo $ora_min_version. | cut -d. -f2`
          test -z "$ora_min_minor" && ora_min_minor=0
          ora_min_micro=`echo $ora_min_version. | cut -d. -f3`
          test -z "$ora_min_micro" && ora_min_micro=0
          ora_min_patchl=`echo $ora_min_version. | cut -d. -f4`
          test -z "$ora_min_patchl" && ora_min_patchl=0

          #split the detected version
          ora_major=`echo $ORACLE_VERSION | cut -d. -f1`
          test -z "$ora_major" && ora_major=0
          ora_minor=`echo $ORACLE_VERSION | cut -d. -f2`
          test -z "$ora_minor" && ora_minor=0
          ora_micro=`echo $ORACLE_VERSION | cut -d. -f3`
          test -z "$ora_micro" && ora_micro=0
          ora_patchl=`echo $ORACLE_VERSION | cut -d. -f4`
          test -z "$ora_patchl" && ora_patchl=0

          if test \( \( $ora_major -gt $ora_min_major \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -gt $ora_min_minor \) \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -eq $ora_min_minor \) -a \( $ora_micro -gt $ora_min_micro \) \) -o \
                \( \( $ora_major -eq $ora_min_major \) -a \( $ora_minor -eq $ora_min_minor \) -a \( $ora_micro -eq $ora_min_micro \) -a \( $ora_patchl -ge $ora_min_patchl \) \) \) ; then
              # this is the minumum required version
              # action-if-found
              ifelse([$3], , :, [$3]) 
          else
              echo "*** Sorry your Oracle version is not sufficient"
              # action-if-not-found
              ifelse([$4], , :, [$4])
          fi
      else
         # ignore version check. Do action-if-found
         ifelse([$3],,:,[$3])
      fi  
  else
      echo "*** Could not detect your oracle version"
      # action-if-not-found
      ifelse([$4], , :, [$4])
  fi

])

dnl-------------------------------------------------------------------------
dnl AM_PATH_ORACLE([MINIMUM-VERSION [, ORACLE_HOME [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Sets the following variables to support Oracle:
dnl - $ORACLE_SHLIBS
dnl - $ORACLE_STLIBS
dnl - $ORACLE_LDFLAGS
dnl - $ORACLE_CPPFLAGS
dnl - $ORACLE_HOME
dnl - $ORACLE_VERSION (via ACX_ORACLE_VERSION)
dnl - $ORACLE_LIBDIR
dnl - $ORACLE_PCC
dnl - $ORACLE_PCCFLAGS
dnl - $ORACLE_PCCINCLUDE
dnl
dnl Requires macro ACX_LD_RUNPATH_SWITH
dnl
AC_DEFUN(AM_PATH_ORACLE, [
AC_ARG_WITH(oracle,
  AC_HELP_STRING([--with-oraclehome[=DIR]], [DIR is Oracle's home directory, defaults to \$ORACLE_HOME.]),
[ORACLEINST_TOP=$withval], 
[ORACLEINST_TOP=ifelse([$2], , $ORACLE_HOME, [$2], yes, $ORACLE_HOME, [$1])])

  NLS_LANG="American_America.WE8ISO8859P1"
  export NLS_LANG

  set -x

  if test "$ORACLEINST_TOP" != ""
  then

    ACX_ORACLE_VERSION([$1], $ORACLEINST_TOP, [

    # Oracle include files
    if test -f "$ORACLEINST_TOP/rdbms/public/ocidfn.h"
    then
      # V8.0.5
      ORACLE_CPPFLAGS="-I$ORACLEINST_TOP/rdbms/public"
    elif test -f "$ORACLEINST_TOP/rdbms/demo/ocidfn.h" ; then
      # V7.[0123]
      ORACLE_CPPFLAGS="-I$ORACLEINST_TOP/rdbms/demo"
    fi

    if test -d "$ORACLEINST_TOP/network/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/network/public"
    fi

    if test -d "$ORACLEINST_TOP/plsql/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/plsql/public"
    fi

    if test -d "$ORACLEINST_TOP/rdbms/public"
    then
      # V8
      ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/rdbms/public"
    fi

    # Oracle libs - nightmare :-)

    ORACLE_LIBDIR=$ORACLEINST_TOP/lib

    ACX_LD_RUNPATH_SWITCH([
      ORACLE_LDFLAGS="-L$ORACLE_LIBDIR ${LD_RUNPATH_SWITCH}$ORACLE_LIBDIR"
    ], [ ORACLE_LDFLAGS="-L$ORACLE_LIBDIR" ])


    if test -f "$ORACLEINST_TOP/rdbms/lib/sysliblist"
    then
      ORA_SYSLIB="`cat $ORACLEINST_TOP/rdbms/lib/sysliblist`"
    else
      ORA_SYSLIB="-lm"
    fi

    # Oracle Static libs and PRO*C flags
    case $ORACLE_VERSION in
      7.0*|7.1*)
        ORACLE_STLIBS="-lsql -locic $ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lnlsrtl -lcv6 -lcore -lnlsrtl -lcv6 \
            -lcore $ORA_SYSLIB "
        if test "`uname -s 2>/dev/null`" = "AIX"; then
            ORACLE_STLIBS="$ORACLE_STLIBS -bI:$ORACLE_HOME/lib/mili.exp"
        fi
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/proc/lib"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes  maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"
        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/proc/lib -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      7.2*)
        ORACLE_STLIBS="-locic $ORACLE_LIBDIR/osntab.o \
            -lsqlnet -lora -lsqlnet -lora -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 \
            -lcore3 $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc16"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/sqllib/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"

        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/sqllib/public -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      7.3*)
        ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
            -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore3 -lnlsrtl3 -lcore3 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc16"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID)"

        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public -DSQLCA_STORAGE_CLASS=static -DSQLCA_INIT"
        ;;
      8.0*)
        ORACLE_STLIBS="-lclient -lsqlnet -lncr -lsqlnet -lclient -lcommon \
            -lgeneric -lsqlnet -lncr -lsqlnet -lclient -lcommon -lgeneric \
            -lepc -lnlsrtl3 -lc3v6 -lcore4 -lnlsrtl3 -lcore4 -lnlsrtl3 \
            $ORA_SYSLIB -lcore3 $ORA_SYSLIB"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID) code=ansi_c lines=yes"
        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public" 
        ;;

      8.1* | 9.0* | * )
        # Don't know if the STLIBS are right :(
        ORACLE_STLIBS="`cat $ORACLE_LIBDIR/ldflags`"
	    ORACLE_PCC="$ORACLEINST_TOP/bin/proc"
        ORACLE_PCCINCLUDE="include=./ include=$ORACLEINST_TOP/precomp/public"
	    ORACLE_PCCFLAGS="define=SQLCA_STORAGE_CLASS=static define=SQLCA_INIT\
                         ireclen=132 oreclen=132 select_error=no ltype=none \
                         hold_cursor=yes parse=none maxopencursors=100\
                         release_cursor=no sqlcheck=\$(ORA_SQLCHECK) \
                        \$(ORA_USERID) code=ansi_c lines=yes"
        ORACLE_CPPFLAGS="$ORACLE_CPPFLAGS -I$ORACLEINST_TOP/precomp/public"
        ;;

    esac
  
    ##################################
    # Oracle shared libs
    ##################################
    case $ORACLE_VERSION in
      7.0*)
        # shared libs not supported
        ORACLE_SHLIBS="$ORACLE_STLIBS"
        ;;
      7.1*)
        if test -f $ORACLE_LIBDIR/liboracle.s?
        then
          ORACLE_SHLIBS="-loracle $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        ;;
      7.2*|7.3*)
        if test -f $ORACLE_LIBDIR/libclntsh.s?
        then
          ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        ;;
      8.0*)
        if test -f $ORACLE_LIBDIR/libclntsh.s? -o \
                -f $ORACLE_LIBDIR/libclntsh.a # AIX
        then
          if test "$CC" = "gcc" -a "`uname -sv`" = "AIX 4"; then
            # for Oracle 8 on AIX 4
            ORA_SYSLIB="$ORA_SYSLIB -nostdlib /lib/crt0_r.o /usr/lib/libpthreads.a /usr/lib/libc_r.a -lgcc"
          fi
          ORACLE_SHLIBS="-lclntsh -lpsa -lcore4 -lnlsrtl3 -lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        AC_DEFINE(HAVE_OCI8)
        ;;

      8.1* | 9.0* | *)
        if test -f $ORACLE_LIBDIR/libclntsh.s? -o \
                -f $ORACLE_LIBDIR/libclntsh.a # AIX
        then
          if test "$CC" = "gcc" -a "`uname -sv`" = "AIX 4"; then
            # for Oracle 8 on AIX 4
            ORA_SYSLIB="$ORA_SYSLIB -nostdlib /lib/crt0_r.o /usr/lib/libpthreads.a /usr/lib/libc_r.a -lgcc"
          fi
          ORACLE_SHLIBS="-lclntsh $ORA_SYSLIB"
        else
          ORACLE_SHLIBS="$ORACLE_STLIBS"
        fi
        AC_DEFINE(HAVE_OCI8)
        ;;
    esac

    # only using shared libs right now
	ORACLE_HOME=$ORACLEINST_TOP

    if test -n "$ORACLE_SHLIBS"; then
       acx_save_libs=$LIBS
       acx_save_ldflags=$LDFLAGS
       LIBS="$ORACLE_SHLIBS $LIBS"
       LDFLAGS="$ORACLE_LDFLAGS $LDFLAGS"
       AC_CACHE_CHECK([linking with oracle shared libs ($ORACLE_SHLIBS) works], 
                      acx_cv_oralink_shared_works, [
          AC_TRY_LINK([],[], acx_cv_oralink_shared_works=yes, acx_cv_oralink_shared_works=no)
          ])
      # This must be possible, otherwise something is really fucked, or
      # this macro doesn't cover your Oracle release.
      if test "$acx_cv_oralink_shared_works" = "no" ; then
         echo
         echo "Cannot link with oracle !!!"
         echo "LDFLAGS=$LDFLAGS"
         echo "LIBS=$LIBS"
         echo
         exit 1
      fi
       LIBS=$acx_save_libs
       LDFLAGS=$acx_save_ldflags
    fi

    # This should be possible at least up to Oracle 8.0.5
    # Didn't figure out 8i yet.
    if test -n "$ORACLE_STLIBS"; then
       acx_save_libs=$LIBS
       acx_save_ldflags=$LDFLAGS
       LIBS="$ORACLE_STLIBS $LIBS"
       LDFLAGS="$ORACLE_LDFLAGS $LDFLAGS"
       AC_CACHE_CHECK([linking with oracle static libs ($ORACLE_STLIBS) works], 
         acx_cv_oralink_static_works, [
          AC_TRY_LINK([],[], acx_cv_oralink_static_works=yes, acx_cv_oralink_static_works=no)
          ])
       LIBS=$acx_save_libs
       LDFLAGS=$acx_save_ldflags
    fi
  AC_SUBST(ORACLE_SHLIBS)
  AC_SUBST(ORACLE_STLIBS)
  AC_SUBST(ORACLE_LDFLAGS)
  AC_SUBST(ORACLE_CPPFLAGS)
  AC_SUBST(ORACLE_HOME)
  AC_SUBST(ORACLE_LIBDIR)
  AC_SUBST(ORACLE_PCC)
  AC_SUBST(ORACLE_PCCFLAGS)
  AC_SUBST(ORACLE_PCCINCLUDE)

  #execute action-if-found
  ifelse([$3],,:,[$3])

], [
dnl did not find the right Oracle Version execute action-if-not-found
ifelse([$4],, :, [$4])
], )
  else
    echo "*** Restart with --with-oraclehome=<path-to-oracle-home> or set \$ORACLE_HOME"
   ifelse([$4],, :, [$4])
  fi
])


dnl $Id$
