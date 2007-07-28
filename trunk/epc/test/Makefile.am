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

GENERATED_FILES      = epctest.c epctest.h epctest.c epctest.pkb epctest.pke epctest.pks epctest.pls
GENERATED_FILES     += str.c str.h str.c str.pkb str.pke str.pks str.pls

MOSTLYCLEANFILES     = $(GENERATED_FILES)

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

EXTRA_DIST           = epctest.ref epctest.sql str.sql test2.client.ref

# test2 is obsolete

#TESTS = test1 test2

TESTS = test1 test2

COUNT = 1000

FORCE = force

EPCTEST = ./epctest
DIFF = diff -w

# Ensure the created dummy test scripts are executable (chmod +x)

test1: $(srcdir)/epctest.sql $(FORCE)
# empty the pipe
	echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/server.sql true
# run the server
	$(EPCTEST) -Dd,g,t,o -u$(USERID) 1>$@.server.log 2>&1 &
	while ! grep "Connected to database...." $@.server.log; do sleep 1; done
	sleep 5
	time echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$< $(COUNT) SOAP 2>&1 | tee $@.client.log
	-@for p in `ps | grep epctest | grep -v grep | awk '{ print $$1; }'`; do kill -INT $$p; sleep 1; kill -KILL $$p; done
	$(DIFF) epctest.lis $(srcdir)/epctest.ref
	@echo : > $@
	@chmod +x $@

test2: $(srcdir)/epctest.sql $(FORCE)
# empty the pipe
	echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/server.sql true
# run the server
	$(EPCTEST) -Dd,g,t,o -u$(USERID) 1>$@.server.log 2>&1 &
	while ! grep "Connected to database...." $@.server.log; do sleep 1; done
	sleep 5
	time echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$< $(COUNT) XMLRPC 2>&1 | tee $@.client.log
	-@for p in `ps | grep epctest | grep -v grep | awk '{ print $$1; }'`; do kill -INT $$p; sleep 1; kill -KILL $$p; done
	$(DIFF) epctest.lis $(srcdir)/epctest.ref
	@echo : > $@
	@chmod +x $@

test3: $(srcdir)/client.sql $(FORCE)
# empty the pipe
	echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/server.sql true
#	sleep 5
	echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$(srcdir)/server.sql false 1>$@.server.log 2>&1 &
	echo exit SQL.SQLCODE | $(SQLPLUS) -s $(USERID) @$< $(COUNT) 1>$@.client.log 2>&1
	-@pid=`ps | grep $(SQLPLUS) | grep -v grep | awk '{ print $$1; }'`; test -z "$$pid" || kill "$$pid"
	$(DIFF) $@.client.log $(srcdir)/$@.client.ref
	@echo : > $@
	@chmod +x $@

force: ;
