## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 G.J. Paulissen
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS     = ansi2knr

MOSTLYCLEANFILES     = epc.c

AM_CPPFLAGS          = -I$(top_srcdir)/src/lib

include_HEADERS      = epc.h epc_defs.h epc_dbg.h idl_defs.h

lib_LTLIBRARIES      = libepc.la

libepc_la_SOURCES    = lsnr.c epc.c epc_call.c epc_dbg.c epc_call.h

##
## History of interface versions:
## - 0:0:0 start
## - 1:0:0 due to add epc_abort and epc_disconnect

libepc_la_LDFLAGS     = -no-undefined -version-info 1:0:1
libepc_la_LIBADD      = @PROCLIB@

EXTRA_DIST           = Imakefile ts_epc-def.msub epc.pc


@SUFFIXES@: .c .pc

.pc.c:
	$(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$< ONAME=$@
