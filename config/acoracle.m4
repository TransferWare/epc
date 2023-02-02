dnl acoracle.m4
dnl
dnl Author: G.J. Paulissen (gert.jan.paulissen@gmail.com)
dnl
dnl Defines the macros:
dnl  ACX_SEARCH_LIBS
dnl  ACX_PROG_PROC
dnl  ACX_PROG_SQLPLUS
dnl  ACX_PROG_XML
dnl

# ACX_SEARCH_LIBS(ROOT-DIRS, SUB-DIRS, FUNCTION, FUNCTION-CALL, SEARCH-LIBS [, ACTION-IF-FOUND
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
# can be found in $ORACLE_HOME/lib (or $ORACLE_HOME/lib32 for 64 bit systems like AIX).
#
# The Cygwin compiler is also able to link to DLLs on a Windows platform.
# However this will likely result in static libraries being built.
#
# The Iterix compiler (Windows Services For Unix from Microsoft) can not link
# to a .lib file via -l. In that case the full path must be added to
# LIBS before calling this macro.

AC_DEFUN([ACX_SEARCH_LIBS],
[AC_CACHE_CHECK([for library containing $3 in directories $1], [acx_cv_search_$3],
[acx_func_search_save_LDFLAGS=$LDFLAGS
acx_func_search_save_LIBS=$LIBS
acx_cv_search_$3=no
acx_LDFLAGS=
acx_LIBS=
AC_LINK_IFELSE([AC_LANG_CALL([], [$3])],
               [acx_cv_search_$3="none required"])
for acx_rootdir in $1; do
  test -d $acx_rootdir || continue
  if test "$acx_cv_search_$3" = no; then
    if test -z "$2"; then
      acx_subdirs=`cd $acx_rootdir; find . -follow \( -name '*.dll' -o -name '*.so' -o -name '*.a' -o -name '*.dylib' \) -exec dirname {} \; | sort -u`
    else
      acx_subdirs=$2
    fi
    for acx_subdir in $acx_subdirs; do
      acx_dir="$acx_rootdir/$acx_subdir"
      test -d $acx_dir || continue
      acx_LDFLAGS="-L${acx_dir}"
      # is $acx_LDFLAGS already part of (original) $LDFLAGS?
      if ! echo "$acx_func_search_save_LDFLAGS" | grep "\\$acx_LDFLAGS" 1>/dev/null; then
        # Use xargs to strip whitespace
        LDFLAGS=`echo "${acx_LDFLAGS} $acx_func_search_save_LDFLAGS" | xargs`
      fi
      if test -z "$5"; then
        # GJP 2018-08-20
        # 1) Get list of libraries and ignore errors due to different Operating Systems
        # 2) Get the basename of the libraries (strip extension)
        acx_libs=`cd $acx_dir && ls *.dll lib*.so lib*.a lib*.dylib 2>/dev/null || true`
        acx_libs=`for f in $acx_libs; do echo $f | sed -E 's/\.dll$//; s/^lib(.*)\.(so|a|dylib)$/\1/'; done`
      else
        acx_libs=$5
      fi    
      for acx_lib in $acx_libs; do
        acx_LIBS="-l$acx_lib $8"
        # is $acx_LIBS already part of (original) $LIBS?
        if ! echo "$acx_func_search_save_LIBS" | grep "\\$acx_LIBS" 1>/dev/null; then
          # Use xargs to strip whitespace
          LIBS=`echo "${acx_LIBS} $acx_func_search_save_LIBS" | xargs`
        fi
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[$acx_prologue]], [[$4]])],
                       [acx_cv_search_$3="$acx_lib" && break],
                       [echo $3 NOT found && echo "conftest.c: `cat conftest.c`"])
      done
      test "$acx_cv_search_$3" = "no" || break
    done
    test "$acx_cv_search_$3" = "no" || break
  fi
done
LDFLAGS=$acx_func_search_save_LDFLAGS
LIBS=$acx_func_search_save_LIBS
])
AS_IF([test "$acx_cv_search_$3" != no],
      [if test "$acx_cv_search_$3" != "none required"
then
  if test -n "$acx_LDFLAGS"; then
    if ! echo "$ORACLE_LDFLAGS" | grep "\\$acx_LDFLAGS" 1>/dev/null; then
      # Use xargs to strip whitespace
      ORACLE_LDFLAGS=`echo "$acx_LDFLAGS $ORACLE_LDFLAGS" | xargs`
    fi
  fi
  if test -n "$acx_LIBS"; then
    if ! echo "$ORACLE_LIBS" | grep "\\$acx_LIBS" 1>/dev/null; then
	    # Use xargs to strip whitespace
      ORACLE_LIBS=`echo "$acx_LIBS $ORACLE_LIBS" | xargs`
    fi
  fi
  # GJP 2018-08-20 Define HAVE_<function>
  # GJP 2023-02-02 Use the Oracle LDFLAGS and LIBS to check for the functions but restore them at the end
  LDFLAGS="$ORACLE_LDFLAGS"
  LIBS="$ORACLE_LIBS"
  AC_CHECK_FUNCS([$3],[],[])
  LDFLAGS=$acx_func_search_save_LDFLAGS
  LIBS=$acx_func_search_save_LIBS
fi
       $6],
      [$7])dnl
])

# ACX_PROG_PROC
# -------------
# Look for the Oracle PRO*C compiler. 
# Sets/updates the following variables:
# - PROC          the full path name of the PRO*C compiler
# - PROCINCLUDES  PRO*C compiler includes including directory
#                 of the PRO*C prototype header.
#                 The -I flags of INCLUDE macros are converted into include=..
# - PROCFLAGS     PRO*C compiler flags.
#                 The -D.. flags of DEFS are converted into define=..
# - ORACLE_CPPFLAGS
# - ORACLE_LDFLAGS
# - ORACLE_LIBS

AC_DEFUN([ACX_PROG_PROC],
[AC_PATH_PROG([PROC], [proc], [AC_MSG_ERROR(proc not found)], ["$ORACLE_HOME/bin:$PATH"])dnl

acx_oracle_home="$ORACLE_HOME"
acx_proc_home=`dirname $PROC`
acx_proc_home=`dirname $acx_proc_home`
if test -z "$acx_oracle_home"
then
  acx_oracle_home=$acx_proc_home
  acx_oracle_homes=$acx_proc_home
else
  acx_oracle_homes="$acx_oracle_home $acx_proc_home"
fi
AC_MSG_NOTICE([Checking for PROC in one of these directories (before removing duplicates): $acx_oracle_homes])
# https://unix.stackexchange.com/questions/353321/remove-all-duplicate-word-from-string-using-shell-script
acx_oracle_homes=`echo "$acx_oracle_homes" | xargs -n1 | sort -u | xargs`
AC_MSG_NOTICE([Checking for PROC in one of these directories (after removing duplicates): $acx_oracle_homes])

# GJP 2023-02-01 Find headers before compiling code to search for functions

# find one of those headers
acx_protohdrs="oratypes.h sqlcpr.h sqlproto.h"
acx_protohdrs_found=""
acx_protohdr=
acx_prologue_file=conftest.prologue
echo "#include <stddef.h>" > $acx_prologue_file
acx_prog_proc_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS=
for dir in $acx_oracle_homes
do
  if ! test -d $dir
  then
    AC_MSG_NOTICE([Directory $dir does not exist])
    continue
  fi
  AC_MSG_NOTICE([Checking PROC prototype headers: $acx_protohdrs])
  for file in $acx_protohdrs 
  do
    # Windows: ignore case
    # Bug 849475: just return one header by using head -1
    acx_protohdr=`find $dir -follow \( -name \*.h -o -name \*.H \) | grep -i $file | head -1 2>/dev/null`

    if test -z "$acx_protohdr"
    then
      AC_MSG_NOTICE([File $file can not be found in directory $dir])
      continue
    fi

    AC_MSG_CHECKING([$acx_protohdr])

    # sqlcpr.h.bak is not right, but SQLCPR.H is (at least on Windows)
    if test `basename "$acx_protohdr" | tr 'A-Z' 'a-z'` = "$file"
    then
      acx_protohdr_dir=`dirname $acx_protohdr`
      # GJP 2018-08-20  PRO*C does not like Cygwin paths so use cygpath -m to convert /cygdrive/c to c:/
      if uname | grep -i 'cygwin' 1>/dev/null
      then
        acx_protohdr_dir=`cygpath -m $acx_protohdr_dir`
      fi
      # See https://github.com/TransferWare/epc/issues/5
      if ! echo "$CPPFLAGS" | grep "\\-I$acx_protohdr_dir" 1>/dev/null; then
        # Use xargs to strip whitespace
        CPPFLAGS=`echo "-I$acx_protohdr_dir $CPPFLAGS" | xargs`
      fi
      AC_MSG_RESULT([yes])
      acx_protohdrs_found="$acx_protohdrs_found $acx_protohdr"
      echo "#include \"$file\"" >> $acx_prologue_file
    else
      AC_MSG_RESULT([no])
      AC_MSG_NOTICE([File $acx_protohdr does not match one of the allowed file names: $acx_protohdrs])
    fi
  done
done

echo "void sig_handler(void) { return; }" >> $acx_prologue_file
acx_prologue=`cat $acx_prologue_file`
rm $acx_prologue_file

AC_MSG_NOTICE([PROC headers found: $acx_protohdrs_found])

# GJP 2023-02-01 Now headers are known ($acx_protohdrs_found) we can compile code to search for functions

ACX_SEARCH_LIBS([$acx_oracle_homes],
                [],
                [sqlglm],
                [unsigned char msg[200]; size_t buf_len = sizeof(&msg); size_t msg_len; 
sqlglm(msg, &buf_len, &msg_len)],
                [],
                [],
                [AC_MSG_ERROR(sqlglm not found)])
ACX_SEARCH_LIBS([$acx_oracle_homes],
                [],
                [osnsui],
                [int handle, err;
extern int osnsui(int *handlp, void (*astp), char * ctx);
err = osnsui(&handle, sig_handler, (char *) 0)],
                [],
                [],
                [AC_MSG_WARN(osnsui not found)])
ACX_SEARCH_LIBS([$acx_oracle_homes],
                [],
                [osncui],
                [int handle, err;
extern int osncui(int handlp);
err = osncui(handle)],
                [],
                [],
                [AC_MSG_WARN(osncui not found)])

# GJP 2018-08-20
#
# In sqlcpr.h this code exists but oratypes.h may not be found:
#
# #ifndef ORATYPES
# #include <oratypes.h>
# #endif
#

# GJP 2022-08-23 The check is better now so remove this
# if ! `echo $acx_protohdrs_found | grep oratypes.h`
# then
#   AC_DEFINE([ORATYPES],[1],[oratypes.h is missing so fake its presence])
# fi  

# See https://github.com/TransferWare/epc/issues/5
# Use COMPILE definition from Makefile for the echo.

AC_CHECK_HEADERS_ONCE([oratypes.h sqlcpr.h sqlproto.h])
ORACLE_CPPFLAGS=$CPPFLAGS
CPPFLAGS=$acx_prog_proc_save_CPPFLAGS
PROCINCLUDES='`echo "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)" | sed "s/ -I/ INCLUDE=/g;s/ -[[^ \t]]*//g"`'
AC_SUBST(PROCINCLUDES)
PROCFLAGS='`echo "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)" | sed "s/ -D/ DEFINE=/g;s/ -[[^ \t]]*//g"`'
PROCFLAGS="$PROCFLAGS CHAR_MAP=VARCHAR2 CODE=ANSI_C PARSE=NONE SQLCHECK=SEMANTICS USERID=\$(USERID)"
AC_SUBST(PROCFLAGS)
AC_SUBST(ORACLE_CPPFLAGS)
AC_SUBST(ORACLE_LDFLAGS)
AC_SUBST(ORACLE_LIBS)
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
acx_proc_home=`dirname $PROC`
acx_proc_home=`dirname $acx_proc_home`
if test -z "$acx_oracle_home"
then
  acx_oracle_home=$acx_proc_home
  acx_oracle_homes=$acx_proc_home
else
  acx_oracle_homes="acx_oracle_home $acx_proc_home"
fi
AC_MSG_NOTICE([Checking for XML headers in one of these directories (before removing duplicates): $acx_oracle_homes])
# https://unix.stackexchange.com/questions/353321/remove-all-duplicate-word-from-string-using-shell-script
acx_oracle_homes=`echo "$acx_oracle_homes" | xargs -n1 | sort -u | xargs`
AC_MSG_NOTICE([Checking for XML headers in one of these directories (after  removing duplicates): $acx_oracle_homes])

ACX_SEARCH_LIBS([$acx_oracle_homes],
                [],
                [xmlinit],
                [],
                [],
                [AC_MSG_WARN(xmlinit not found)])

# oraxml.h is deprecated, use xml.h now
acx_xmlhdrs="oratypes.h xml.h oraxml.h"
AC_MSG_NOTICE([Checking headers for XML C SDK: $acx_xmlhdrs])
for acx_file in $acx_xmlhdrs
do
  # Windows: ignore case
  if printenv WINDIR 1>/dev/null
  then
    acx_file_upper=`echo $acx_file | tr 'a-z'     'A-Z'`
  else
    acx_file_upper=$acx_file
  fi

  for acx_dir in $acx_oracle_home/xdk/include $acx_oracle_home/xdk/c/parser/include $acx_oracle_home `find $acx_oracle_home -follow -name include -type d 2>/dev/null` /usr/local/include
  do
    # Must be a directory and we must be able to change to the directory
    test -d $acx_dir -a -x $acx_dir || continue

    AC_MSG_CHECKING([$acx_file in $acx_dir])

    if test -f "$acx_dir/$acx_file"
    then
      acx_xmlhdr="$acx_dir/$acx_file"
    elif test -f "$acx_dir/$acx_file_upper"
    then
      acx_xmlhdr="$acx_dir/$acx_file_upper"
    else
      acx_xmlhdr=
    fi

    if test -n "$acx_xmlhdr"
    then
      if ! echo "$CPPFLAGS" | grep "\\-I${acx_dir}" 1>/dev/null; then
        # Use xargs to strip whitespace
        CPPFLAGS=`echo "-I${acx_dir} $CPPFLAGS" | xargs`
      fi
      AC_MSG_RESULT([yes])
      break
    else
      AC_MSG_RESULT([no])
      continue
    fi
  done
done

# to prevent: fatal error: orastruc.h: No such file or directory
# CPPFLAGS="-DORASTRUC $CPPFLAGS"
for acx_var in CPPFLAGS LDFLAGS LIBS
do
  AC_MSG_NOTICE([Setting environment variable $acx_var])
  export $acx_var
  env | grep $acx_var
done

# GJP 2022-08-23
# Use old preprocessor check, i.e. only existence of the header ([-])
# AC_CHECK_HEADERS([oratypes.h xml.h oraxml.h], [], [], [-])
AC_CHECK_HEADERS_ONCE([oratypes.h xml.h oraxml.h])
])

# ACX_PROG_OCI
# -------------
# Look for the Oracle OCI header.
# Sets/updates the following variables:
# - CPPFLAGS      extended with the OCI header

AC_DEFUN([ACX_PROG_OCI],
[
acx_oracle_home="$ORACLE_HOME"
if test -z "$acx_oracle_home"
then
  acx_oracle_home=`dirname $PROC`
  acx_oracle_home=`dirname $acx_oracle_home`
fi

acx_ocihdrs="oci.h"
acx_ocihdr=
for dir in $acx_oracle_home/rdbms/public `find $acx_oracle_home -follow -name include -type d 2>/dev/null` $acx_oracle_home
do
  # Must be a directory and we must be able to change to the directory
  test -d $dir -a -x $dir || continue
  for file in $acx_ocihdrs
  do
    # Windows: ignore case
    # Bug 849475: just return one header by using head -1
    acx_ocihdr=`find $dir -follow \( -name \*.h -o -name \*.H \) | grep -i $file | head -1 2>/dev/null`

    test -n "$acx_ocihdr" || continue

    AC_MSG_CHECKING([$acx_ocihdr])

    # oraoci.h.bak is not right, but ORAOCI.H is (at least on Windows)
    if test `basename "$acx_ocihdr" | tr 'A-Z' 'a-z'` = "$file"
    then
      CPPFLAGS="-I`dirname $acx_ocihdr` $CPPFLAGS"
      AC_MSG_RESULT([yes])
      break
    else
      acx_ocihdr=
      AC_MSG_RESULT([no])
      continue
    fi
  done
  test -z "$acx_ocihdr" || break
done

AC_CHECK_HEADERS([oci.h], [continue], [AC_MSG_ERROR(OCI header(s) $acx_ocihdrs not found)])
])

