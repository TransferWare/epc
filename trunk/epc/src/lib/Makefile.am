## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 G.J. Paulissen
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
## - 2:0:0 due to adding epc_info to epc_disconnect

libepc_la_LDFLAGS     = -no-undefined -version-info 2:0:0
libepc_la_LIBADD      = @PROCLIB@

EXTRA_DIST           = Imakefile ts_epc-def.msub epc.pc


@SUFFIXES@: .c .pc

.pc.c:
	$(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$< ONAME=$@
