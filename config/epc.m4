## ---------------------------------------- ##
## EPC M4 macros for use in other projects. ##
## From Gert-Jan Paulissen                  ##
## ---------------------------------------- ##

# This file is part of epc.
#
# Epc is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Epc is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with epc.  If not, see <http://www.gnu.org/licenses/>.

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
