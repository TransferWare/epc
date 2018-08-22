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

BUILT_SOURCES        = epctest.h epctest.c epctest.pkb epctest.pke epctest.pks epctest.pls
BUILT_SOURCES       += str.h str.c str.pkb str.pke str.pks str.pls

MOSTLYCLEANFILES     = $(BUILT_SOURCES) *.pc *.lis

TESTS                = test.native

if XML

TESTS               += test.soap test.xmlrpc

endif

CLEANFILES           = $(TESTS) *.log epctest.lis *.pls_run *.idl_run

# AUTOMAKE_OPTIONS     = $(top_builddir)/src/lib/ansi2knr

AM_CPPFLAGS          = -I$(top_srcdir)/src/lib @DBUG_CPPFLAGS@

EXTRA_DIST           = epctest.ref epctest.sql str.sql

check_DATA           = epctest.pls_run
check_PROGRAMS       = epctest
epctest_SOURCES      = epctest.idl epctest.c str.idl str.c proc.c main.c
epctest_LDADD        = $(top_builddir)/src/lib/libepc.la @DBUG_LDADD@

IDL                  = $(top_builddir)/src/prog/idl$(EXEEXT)

@SUFFIXES@           = .pls_run .pls .idl_run .idl

.idl.idl_run:
	$(IDL) $<
	@touch $@

.pls.pls_run:
# Before running the compiler first compile the package
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$<
	@touch $@

$(BUILT_SOURCES): epctest.idl_run str.idl_run

epctest.idl_run str.idl_run: $(IDL)

$(IDL):
	cd $(top_builddir)/src/prog && $(MAKE) all

$(top_builddir)/src/lib/libepc.la:
	cd $(top_builddir)/src/lib && $(MAKE) all

##
## Test software
##

COUNT = 500

FORCE = force

EPCTEST = ./epctest
DIFF = diff -w

# run the server in the background and purge the request pipe
START_SERVER = @PERL@ $(top_builddir)/src/prog/epcdaemon.pl \
               --verbose $(EPCTEST) -P -Dd,g,t,o -u$(USERID)

STOP_SERVER = echo "Killing server at `date`" && $(EPCTEST) -I -u$(USERID)

# Ensure the created dummy test scripts are executable (chmod +x)

test.soap: $(FORCE)
	$(START_SERVER) 1>$@.server.log 2>&1
	while ! grep "Connected to database...." $@.server.log; do sleep 1; done
	sleep 5
	time echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/epctest.sql $(COUNT) SOAP 2>&1 | tee $@.client.log
	@$(STOP_SERVER)
	$(DIFF) epctest.lis $(srcdir)/epctest.ref
	@echo : > $@ && chmod +x $@

test.xmlrpc: $(FORCE)
	$(START_SERVER) 1>$@.server.log 2>&1
	while ! grep "Connected to database...." $@.server.log; do sleep 1; done
	sleep 5
	time echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/epctest.sql $(COUNT) XMLRPC 2>&1 | tee $@.client.log
	@$(STOP_SERVER)
	$(DIFF) epctest.lis $(srcdir)/epctest.ref
	@echo : > $@ && chmod +x $@

test.native: $(FORCE)
	$(START_SERVER) 1>$@.server.log 2>&1
	while ! grep "Connected to database...." $@.server.log; do sleep 1; done
	sleep 5
	time echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/epctest.sql $(COUNT) NATIVE 2>&1 | tee $@.client.log
	@$(STOP_SERVER)
	$(DIFF) epctest.lis $(srcdir)/epctest.ref
	@echo : > $@ && chmod +x $@

force: ;
