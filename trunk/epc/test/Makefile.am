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

GENERATED_FILES      = epctest.pc epctest.c epctest.h epctest.c epctest.pkb epctest.pke epctest.pks epctest.pls
GENERATED_FILES     += str.pc str.c str.h str.c str.pkb str.pke str.pks str.pls

MOSTLYCLEANFILES     = $(GENERATED_FILES)

CLEANFILES           = $(TESTS) *.log epctest.lis

AUTOMAKE_OPTIONS     = $(top_builddir)/src/lib/ansi2knr

AM_CPPFLAGS	     = -I$(top_srcdir)/src/lib -I$(includedir)

check_PROGRAMS       = epctest
epctest_SOURCES      = epctest.idl str.idl proc.c main.c
epctest_LDADD        = $(top_builddir)/src/lib/libepc.la

IDL = $(top_builddir)/src/prog/idl$(EXEEXT)

@SUFFIXES@: .c .idl

.idl.c:
	$(IDL) $<
# first compile the package
	echo exit SQL.SQLCODE | $(SQLPLUS) $(USERID) @$(@:.c=.pls)
	test -z "$(PROC)" || $(PROC) $(PROCINCLUDES) $(PROCFLAGS) INAME=$(@:.c=.pc) ONAME=$@

EXTRA_DIST           = epctest.ref epctest.sql str.sql

TESTS = test.soap test.xmlrpc test.native

COUNT = 1000

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
