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


# ACX_SEARCH_LIBS
# ---------------
# Similar to AC_SEARCH_LIBS but sets the full path name of the library instead
# instead of modifying $LIBS.
#
# This macro has been developed because the Oracle libraries on a Windows
# platform may reside in various subdirectories who also might differ with
# respect to their case. For instance the orasql8 library may be found
# in $ORACLE_HOME/precomp/lib/msvc, but also in $ORACLE_HOME/precomp/lib.
# On Unix systems the library is called clntsh and can be found in 
# $ORACLE_HOME/lib.
#
# The Cygwin compiler is also able to link to DLLs on a Windows platform.
# However this will likely result in static libraries being built.
#
# The Iterix compiler (Windows Services For Unix from Microsoft) can not link
# to a .lib file via -l. In that case the full path must be known.
#
# Parameters:
#
# - $1            The root directory.
# - $2            A list of subdirectories to be searched for the library.
# - $3            The function to be found.
# - $4            A list of library base names. On Unix the actual file name 
#                 is lib$base (without suffix). On Windows it might also be
#                 $base (no suffix).
# - $5            The name of the shell output variable which is set to the full
#                 path name
#
# The algorithm to search for a library:
# 1) Find all subdirectories relative to the root (ignore case)
# 2) If any library ${base} contains the function, this library is searched for
#    in the directory tree according to the following rules:
#    a) traverse each subdirectory found in step 1
#       Next stop when
#       1) lib${base}.la exists as link or normal file, or
#    	2) lib${base}.so exists as link or normal file, or
#    	3) lib${base}.a exists as link or normal file, or
#    	4) ${base}.lib exists as link or normal file, or
#    	5) ${base}.dll exists as link or normal file
#       If the full library path is not found, raise an error
#    b) Set the full path name to $5
#
AC_DEFUN(
[ACX_SEARCH_LIBS],
[
acx_root_dir=$1
acx_subdirs="$2"
acx_function=$3
acx_libs="$4"
acx_var=$5

# Construct LDFLAGS
acx_save_ld_flags="$LDFLAGS"
acx_dirs=""

for acx_subdir in $acx_subdirs
do
  acx_dir=""
  for acx_dir in "$acx_root_dir/$acx_subdir" \
                 "$acx_root_dir/`echo $acx_subdir | tr '[[:lower:]]' '[[:upper:]]'`"
  do
    if test -d "$acx_dir"
    then
      LDFLAGS="$LDFLAGS -L$acx_dir"
      acx_dirs="$acx_dirs $acx_dir"
      break
    fi
  done
done

acx_lib_found=""
for acx_lib in $acx_libs
do
  AC_CHECK_LIB([$acx_lib],
               [$acx_function],
               [acx_lib_found=$acx_lib && break],
               [])
done
LDFLAGS="$acx_save_ldflags"

acx_lib_pathname=""
if test -n "$acx_lib_found"
then
  AC_MSG_CHECKING([for the full pathname of the library for $acx_function])
  for acx_dir in ${acx_dirs:?"acx_dirs undefined"}
  do
    for acx_prefix_suffix in lib:.la lib:.so lib:.a :.lib :.dll
    do
      acx_prefix=`echo $acx_prefix_suffix | cut -d':' -f1`
      acx_suffix=`echo $acx_prefix_suffix | cut -d':' -f2`
      # Bug 849475
      # Check links first, next normal files
      for type in l f
      do
        acx_lib_pathname=`find $acx_dir -type $type | ${EGREP:?"EGREP undefined"} -i "^$acx_dir/$acx_prefix$acx_lib_found$acx_suffix$" | 
head -1`
        test -z "$acx_lib_pathname" || break
      done
      test -z "$acx_lib_pathname" || break
    done
    test -z "$acx_lib_pathname" || break
  done
  if test -z "$acx_lib_pathname"
  then
    AC_MSG_RESULT([])
    AC_MSG_ERROR([Could not find full path for library $acx_lib_found in $acx_dirs])
  else
    AC_MSG_RESULT([$acx_lib_pathname])
  fi
else
  AC_MSG_WARN([Could not find one of the libraries $acx_libs])
fi
eval $acx_var="$acx_lib_pathname"
dnl
])

# ACX_PROG_PROC
# -------------
# Look for the Oracle PRO*C compiler. 
# Sets/updates the following variables:
# - PROC          the full path name of the PRO*C compiler
# - PROCLIB       its associated libraries (-L<libclntsh directory> -lclntsh for Unix)
#                 where functions sqlglm and osnsui have been found.
# - PROCFLAGS     PRO*C compiler flags.
#                 The -D.. flags of CPPFLAGS are converted into define=..
# - PROCINCLUDES  PRO*C compiler includes including directory
#                 of the PRO*C prototype header.
#                 The -I flags of CPPFLAGS are converted into include=..

#AC_DEFUN([ACX_PROG_PROC],
#[AC_REQUIRE(AC_CANONICAL_HOST)dnl

AC_DEFUN([ACX_PROG_PROC],
[AC_PATH_PROG([PROC], [proc], [AC_MSG_ERROR(proc not found)])dnl

acx_oracle_home="$ORACLE_HOME"
if test -z "$acx_oracle_home"
then
  acx_oracle_home=`dirname $PROC`
  acx_oracle_home=`dirname $acx_oracle_home`
fi

ACX_SEARCH_LIBS([$acx_oracle_home],
                [lib precomp precomp/lib precomp/lib/msvc bin],
                [sqlglm],
                [clntsh orasql9 orasql8 orasql7],
                [acx_lib_sqlglm])
ACX_SEARCH_LIBS([$acx_oracle_home],
                [lib precomp precomp/lib precomp/lib/msvc bin],
                [osnsui],
                [clntsh oran9 oran8 oran7],
                [acx_lib_osnsui])

PROCLIB="$acx_lib_sqlglm $acx_lib_osnsui"
AC_SUBST(PROCLIB)

acx_prochdr=
for dir in $acx_oracle_home/precomp/public $acx_oracle_home
do
  for file in sqlcpr.h sqlproto.h
  do
    # Windows: ignore case
    # Bug 849475: just return one header by using head -1
    acx_protohdr=`find $dir \( -name \*.h -o -name \*.H \) | grep -i $file | head -1 2>/dev/null`
    test -z "$acx_protohdr" || break
  done
  test -z "$acx_protohdr" || break
done

CPPFLAGS="-I`dirname $acx_protohdr` $CPPFLAGS"
acx_protohdr=`basename $acx_protohdr`
AC_CHECK_HEADERS([sqlcpr.h sqlproto.h], [break], [AC_MSG_ERROR(PRO*C prototype header not found)])
AC_MSG_CHECKING(for the PRO*C prototype header)
AC_MSG_RESULT($acx_protohdr)

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
  acx_path=`dirname $PROC`
elif test -n "${ORACLE_HOME:=}"
then
  acx_path="$ORACLE_HOME/bin"
else
  acx_path="$PATH"
fi

AC_PATH_PROGS([SQLPLUS], [plus80 plus33 plus32 plus31 sqlplus], [], [$acx_path])
test -n "$SQLPLUS" || AC_MSG_ERROR(sqlplus not found)
])

dnl $Id$
