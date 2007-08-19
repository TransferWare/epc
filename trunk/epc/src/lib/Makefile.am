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

include $(top_srcdir)/lint.am

# too difficult to lint epc_xml.c
#epc_xml_ln = epc_xml.ln

#$(epc_xml_ln): ;

epc_xml.ln: epc_xml.c
	$(LINT_c) +posixlib $<


MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS     = ansi2knr

MOSTLYCLEANFILES     = epc.c

AM_CPPFLAGS          = -I$(top_srcdir)/src/lib @DBUG_CPPFLAGS@

include_HEADERS      = epc.h epc_defs.h idl_defs.h

lib_LTLIBRARIES      = libepc.la

libepc_la_SOURCES    = lsnr.c epc.pc epc_xml.c epc_xml.h epc_lib.c epc_lib.h

##
## History of interface versions:
## - 0:0:0 start
## - 1:0:0 due to add epc_abort and epc_disconnect
## - 2:0:0 due to adding epc_info to epc_disconnect
## - 3:0:0 2004-10-21 funtion namespace changed: epc_ -> epc__
## - 4:0:0 2004-12-16 1) result paramater renamed to return conform XML SOAP.
##                    2) namespace introduced (used by web services)
## - 5:0:0 2007-08-07 readding dbms_pipe support (no XML data just the Oracle interface)

libepc_la_LDFLAGS     = -no-undefined -version-info 5:0:0
libepc_la_LIBADD      = $(top_builddir)/replace/libreplace.la @DBUG_LIBADD@

.pc.c:
	$(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$< ONAME=$@
