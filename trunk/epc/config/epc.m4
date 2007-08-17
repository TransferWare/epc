## ----------------------------------- ##
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
LIBS="-lepc $LIBS"
AC_CHECK_FUNC([epc__init],[],[AC_MSG_ERROR(epc__init not found)])
])
