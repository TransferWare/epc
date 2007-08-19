## ----------------------------------- ##
## DBUG M4 macros for use in other projects. ##
## From Gert-Jan Paulissen             ##
## ----------------------------------- ##

# Copyright 1996, 1998, 1999, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([ACX_EPC],
[AC_REQUIRE([ACX_DBUG])
AC_PREFIX_PROGRAM([epcdaemon])
AC_PATH_PROG([EPCDAEMON],[epcdaemon])
acx_epcdaemon_dir=`dirname $EPCDAEMON`
acx_epcdaemon_dir=`dirname $acx_epcdaemon_dir`
AC_SUBST([EPC_LIBADD],[${acx_epcdaemon_dir}/lib/libepc.la])
AC_SUBST([EPC_LDADD],[${acx_epcdaemon_dir}/lib/libepc.la])
AC_SUBST([EPC_CPPFLAGS],[-I${acx_epcdaemon_dir}/include])
])
