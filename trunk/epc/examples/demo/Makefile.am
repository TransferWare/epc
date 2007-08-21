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

BUILT_SOURCES        = demo.h demo.c demo.pkb \
                       demo.pke demo.pks demo.pls

MOSTLYCLEANFILES     = $(BUILT_SOURCES) *.pc *.lis

CLEANFILES           = *.pls_run *.idl_run

AUTOMAKE_OPTIONS     = $(top_builddir)/src/lib/ansi2knr

AM_CPPFLAGS          = -I$(top_srcdir)/src/lib @DBUG_CPPFLAGS@

# Just the SQL*Plus scripts
EXTRA_DIST           = demo.pkb demo.pks demo.pls

# Install the SQL*Plus package before building
noinst_DATA          = demo.pls_run

noinst_PROGRAMS      = demo
demo_SOURCES         = demo.idl demo.c main.c
demo_LDADD           = $(top_builddir)/src/lib/libepc.la @DBUG_LDADD@

IDL                  = $(top_builddir)/src/prog/idl$(EXEEXT)

@SUFFIXES@           = .pls_run .pls .idl_run .idl

.idl.idl_run:
	$(IDL) $<
	@touch $@

.pls.pls_run:
# Before running the compiler first compile the package
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$<
	@touch $@

$(BUILT_SOURCES): demo.idl_run

demo.idl_run: $(IDL)

$(IDL):
	cd $(top_builddir)/src/prog && $(MAKE) all

$(top_builddir)/src/lib/libepc.la:
	cd $(top_builddir)/src/lib && $(MAKE) all
