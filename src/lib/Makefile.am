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

include ../../lint.am

# too difficult to lint epc_xml.c
epc_xml.ln: ;

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS     = ansi2knr

MOSTLYCLEANFILES     = epc.c

AM_CPPFLAGS          = -I$(top_srcdir)/src/lib -I$(includedir)

include_HEADERS      = epc.h epc_defs.h idl_defs.h

lib_LTLIBRARIES      = libepc.la

libepc_la_SOURCES    = lsnr.c epc.pc epc_xml.c epc_xml.h epc_lib.c

##
## History of interface versions:
## - 0:0:0 start
## - 1:0:0 due to add epc_abort and epc_disconnect
## - 2:0:0 due to adding epc_info to epc_disconnect

libepc_la_LDFLAGS     = -no-undefined -version-info 2:0:0
libepc_la_LIBADD      = $(libdir)/libdbug.la

.pc.c:
	$(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$< ONAME=$@
